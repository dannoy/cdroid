#include <unistd.h>
#include "options.h"
#include <string.h>
#include "aidl_language.h"
#include "aidl_common.h"
#include "generate_java.h"
#include "Type.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>

#ifdef HAVE_MS_C_RUNTIME
#include <io.h>
#endif

static int
gather_types(const char* filename, document_item_type* items)
{
    int err = 0;
    while (items) {
        Type* type;
        if (items->item_type == USER_DATA_TYPE) {
            user_data_type* p = (user_data_type*)items;
            type = new UserDataType(p->package ? p->package : "", p->name.data,
                    false, ((p->flattening_methods & PARCELABLE_DATA) != 0),
                    ((p->flattening_methods & RPC_DATA) != 0), filename, p->name.lineno);
        }
        else if (items->item_type == INTERFACE_TYPE_BINDER
                || items->item_type == INTERFACE_TYPE_RPC) {
            interface_type* c = (interface_type*)items;
            type = new InterfaceType(c->package ? c->package : "",
                            c->name.data, false, c->oneway,
                            filename, c->name.lineno);
        }
        else {
            fprintf(stderr, "aidl: internal error %s:%d\n", __FILE__, __LINE__);
            return 1;
        }

        Type* old = NAMES.Find(type->QualifiedName());
        if (old == NULL) {
            NAMES.Add(type);

            if (items->item_type == INTERFACE_TYPE_BINDER) {
                // for interfaces, also add the stub and proxy types, we don't
                // bother checking these for duplicates, because the parser
                // won't let us do it.
                interface_type* c = (interface_type*)items;

                string name = c->name.data;
                name += ".Stub";
                Type* stub = new Type(c->package ? c->package : "",
                                        name, Type::GENERATED, false, false, false,
                                        filename, c->name.lineno);
                NAMES.Add(stub);

                name = c->name.data;
                name += ".Stub.Proxy";
                Type* proxy = new Type(c->package ? c->package : "",
                                        name, Type::GENERATED, false, false, false,
                                        filename, c->name.lineno);
                NAMES.Add(proxy);
            }
            else if (items->item_type == INTERFACE_TYPE_RPC) {
                // for interfaces, also add the service base type, we don't
                // bother checking these for duplicates, because the parser
                // won't let us do it.
                interface_type* c = (interface_type*)items;

                string name = c->name.data;
                name += ".ServiceBase";
                Type* base = new Type(c->package ? c->package : "",
                                        name, Type::GENERATED, false, false, false,
                                        filename, c->name.lineno);
                NAMES.Add(base);
            }
        } else {
            if (old->Kind() == Type::BUILT_IN) {
                fprintf(stderr, "%s:%d attempt to redefine built in class %s\n",
                            filename, type->DeclLine(),
                            type->QualifiedName().c_str());
                err = 1;
            }
            else if (type->Kind() != old->Kind()) {
                const char* oldKind = kind_to_string(old->Kind());
                const char* newKind = kind_to_string(type->Kind());

                fprintf(stderr, "%s:%d attempt to redefine %s as %s,\n",
                            filename, type->DeclLine(),
                            type->QualifiedName().c_str(), newKind);
                fprintf(stderr, "%s:%d    previously defined here as %s.\n",
                            old->DeclFile().c_str(), old->DeclLine(), oldKind);
                err = 1;
            }
        }

        items = items->next;
    }
    return err;
}

// ==========================================================
static bool
matches_keyword(const char* str)
{
    static const char* KEYWORDS[] = { "abstract", "assert", "boolean", "break",
        "byte", "case", "catch", "char", "class", "const", "continue",
        "default", "do", "double", "else", "enum", "extends", "final",
        "finally", "float", "for", "goto", "if", "implements", "import",
        "instanceof", "int", "interface", "long", "native", "new", "package",
        "private", "protected", "public", "return", "short", "static",
        "strictfp", "super", "switch", "synchronized", "this", "throw",
        "throws", "transient", "try", "void", "volatile", "while",
        "true", "false", "null",
        NULL
    };
    const char** k = KEYWORDS;
    while (*k) {
        if (0 == strcmp(str, *k)) {
            return true;
        }
        k++;
    }
    return false;
}

static int
jcheck_method(const char* filename, int kind, method_type* m)
{
    int err = 0;

    // return type
    Type* returnType = NAMES.Search(m->type.type.data);
    if (returnType == NULL) {
        fprintf(stderr, "%s:%d unknown return type %s\n", filename,
                    m->type.type.lineno, m->type.type.data);
        err = 1;
        return err;
    }

    if (returnType == EVENT_FAKE_TYPE) {
        if (kind != INTERFACE_TYPE_RPC) {
            fprintf(stderr, "%s:%d event methods only supported for rpc interfaces\n",
                    filename, m->type.type.lineno);
            err = 1;
        }
    } else {
        if (!(kind == INTERFACE_TYPE_BINDER ? returnType->CanWriteToParcel()
                    : returnType->CanWriteToRpcData())) {
            fprintf(stderr, "%s:%d return type %s can't be marshalled.\n", filename,
                        m->type.type.lineno, m->type.type.data);
            err = 1;
        }
    }

    if (m->type.dimension > 0 && !returnType->CanBeArray()) {
        fprintf(stderr, "%s:%d return type %s%s can't be an array.\n", filename,
                m->type.array_token.lineno, m->type.type.data,
                m->type.array_token.data);
        err = 1;
    }

    if (m->type.dimension > 1) {
        fprintf(stderr, "%s:%d return type %s%s only one"
                " dimensional arrays are supported\n", filename,
                m->type.array_token.lineno, m->type.type.data,
                m->type.array_token.data);
        err = 1;
    }

    int index = 1;

    arg_type* arg = m->args;
    while (arg) {
        Type* t = NAMES.Search(arg->type.type.data);

        // check the arg type
        if (t == NULL) {
            fprintf(stderr, "%s:%d parameter %s (%d) unknown type %s\n",
                    filename, m->type.type.lineno, arg->name.data, index,
                    arg->type.type.data);
            err = 1;
            goto next;
        }

        if (t == EVENT_FAKE_TYPE) {
            fprintf(stderr, "%s:%d parameter %s (%d) event can not be used as a parameter %s\n",
                    filename, m->type.type.lineno, arg->name.data, index,
                    arg->type.type.data);
            err = 1;
            goto next;
        }
        
        if (!(kind == INTERFACE_TYPE_BINDER ? t->CanWriteToParcel() : t->CanWriteToRpcData())) {
            fprintf(stderr, "%s:%d parameter %d: '%s %s' can't be marshalled.\n",
                        filename, m->type.type.lineno, index,
                        arg->type.type.data, arg->name.data);
            err = 1;
        }

        if (returnType == EVENT_FAKE_TYPE
                && convert_direction(arg->direction.data) != IN_PARAMETER) {
            fprintf(stderr, "%s:%d parameter %d: '%s %s' All paremeters on events must be 'in'.\n",
                    filename, m->type.type.lineno, index,
                    arg->type.type.data, arg->name.data);
            err = 1;
            goto next;
        }

        if (arg->direction.data == NULL
                && (arg->type.dimension != 0 || t->CanBeOutParameter())) {
            fprintf(stderr, "%s:%d parameter %d: '%s %s' can be an out"
                                " parameter, so you must declare it as in,"
                                " out or inout.\n",
                        filename, m->type.type.lineno, index,
                        arg->type.type.data, arg->name.data);
            err = 1;
        }

        if (convert_direction(arg->direction.data) != IN_PARAMETER
                && !t->CanBeOutParameter()
                && arg->type.dimension == 0) {
            fprintf(stderr, "%s:%d parameter %d: '%s %s %s' can only be an in"
                            " parameter.\n",
                        filename, m->type.type.lineno, index,
                        arg->direction.data, arg->type.type.data,
                        arg->name.data);
            err = 1;
        }

        if (arg->type.dimension > 0 && !t->CanBeArray()) {
            fprintf(stderr, "%s:%d parameter %d: '%s %s%s %s' can't be an"
                    " array.\n", filename,
                    m->type.array_token.lineno, index, arg->direction.data,
                    arg->type.type.data, arg->type.array_token.data,
                    arg->name.data);
            err = 1;
        }

        if (arg->type.dimension > 1) {
            fprintf(stderr, "%s:%d parameter %d: '%s %s%s %s' only one"
                    " dimensional arrays are supported\n", filename,
                    m->type.array_token.lineno, index, arg->direction.data,
                    arg->type.type.data, arg->type.array_token.data,
                    arg->name.data);
            err = 1;
        }

        // check that the name doesn't match a keyword
        if (matches_keyword(arg->name.data)) {
            fprintf(stderr, "%s:%d parameter %d %s is named the same as a"
                    " Java or aidl keyword\n",
                    filename, m->name.lineno, index, arg->name.data);
            err = 1;
        }
        
next:
        index++;
        arg = arg->next;
    }

    return err;
}


// ==========================================================
void
generate_dep_file(const Options& options, const document_item_type* items)
{
    /* we open the file in binary mode to ensure that the same output is
     * generated on all platforms !!
     */
    FILE* to = NULL;
    if (options.autoDepFile) {
        string fileName = options.outputFileName + ".d";
        to = fopen(fileName.c_str(), "wb");
    } else {
        to = fopen(options.depFileName.c_str(), "wb");
    }

    if (to == NULL) {
        return;
    }

    const char* slash = "\\";
    import_info* import = g_imports;
    if (import == NULL) {
        slash = "";
    }

    if (items->item_type == INTERFACE_TYPE_BINDER || items->item_type == INTERFACE_TYPE_RPC) {
        fprintf(to, "%s: \\\n", options.outputFileName.c_str());
    } else {
        // parcelable: there's no output file.
        fprintf(to, " : \\\n");
    }
    fprintf(to, "  %s %s\n", options.inputFileName.c_str(), slash);

    while (import) {
        if (import->next == NULL) {
            slash = "";
        }
        if (import->filename) {
            fprintf(to, "  %s %s\n", import->filename, slash);
        }
        import = import->next;
    }

    fprintf(to, "\n");

    fclose(to);
}

// ==========================================================
static string
generate_outputFileName2(const Options& options, const buffer_type& name, const char* package)
{
    string result;

    // create the path to the destination folder based on the
    // interface package name
    result = options.outputBaseFolder;
    result += OS_PATH_SEPARATOR;

    string packageStr = package;
    size_t len = packageStr.length();
    for (size_t i=0; i<len; i++) {
        if (packageStr[i] == '.') {
            packageStr[i] = OS_PATH_SEPARATOR;
        }
    }

    result += packageStr;

    // add the filename by replacing the .aidl extension to .java
    const char* p = strchr(name.data, '.');
    len = p ? p-name.data : strlen(name.data);

    result += OS_PATH_SEPARATOR;
    result.append(name.data, len);
    result += ".java";

    return result;
}

// ==========================================================
static string
generate_outputFileName(const Options& options, const document_item_type* items)
{
    // items has already been checked to have only one interface.
    if (items->item_type == INTERFACE_TYPE_BINDER || items->item_type == INTERFACE_TYPE_RPC) {
        interface_type* type = (interface_type*)items;

        return generate_outputFileName2(options, type->name, type->package);
    } else if (items->item_type == USER_DATA_TYPE) {
        user_data_type* type = (user_data_type*)items;
        return generate_outputFileName2(options, type->name, type->package);
    }

    // I don't think we can come here, but safer than returning NULL.
    string result;
    return result;
}

// ==========================================================
int
jcompile_aidl(Options& options)
{
    int err = 0, N;

    set_import_paths(options.importPaths);

    jregister_base_types();

    // import the preprocessed file
    N = options.preprocessedFiles.size();
    for (int i=0; i<N; i++) {
        const string& s = options.preprocessedFiles[i];
        err |= parse_preprocessed_file(s, gather_types);
    }
    if (err != 0) {
        return err;
    }

    // parse the main file
    g_callbacks = &g_mainCallbacks;
    err = parse_aidl(options.inputFileName.c_str());
    document_item_type* mainDoc = g_document;
    g_document = NULL;

    // parse the imports
    g_callbacks = &g_mainCallbacks;
    import_info* import = g_imports;
    while (import) {
        if (NAMES.Find(import->neededClass) == NULL) {
            import->filename = find_import_file(import->neededClass);
            if (!import->filename) {
                fprintf(stderr, "%s:%d: couldn't find import for class %s\n",
                        import->from, import->statement.lineno,
                        import->neededClass);
                err |= 1;
            } else {
                err |= parse_aidl(import->filename);
                import->doc = g_document;
                if (import->doc == NULL) {
                    err |= 1;
                }
            }
        }
        import = import->next;
    }
    // bail out now if parsing wasn't successful
    if (err != 0 || mainDoc == NULL) {
        //fprintf(stderr, "aidl: parsing failed, stopping.\n");
        return 1;
    }

    // complain about ones that aren't in the right files
    err |= check_filenames(options.inputFileName.c_str(), mainDoc);
    import = g_imports;
    while (import) {
        err |= check_filenames(import->filename, import->doc);
        import = import->next;
    }

    // gather the types that have been declared
    err |= gather_types(options.inputFileName.c_str(), mainDoc);
    import = g_imports;
    while (import) {
        err |= gather_types(import->filename, import->doc);
        import = import->next;
    }

#if 0
    printf("---- main doc ----\n");
    test_document(mainDoc);

    import = g_imports;
    while (import) {
        printf("---- import doc ----\n");
        test_document(import->doc);
        import = import->next;
    }
    NAMES.Dump();
#endif

    // check the referenced types in mainDoc to make sure we've imported them
    err |= check_types(options.inputFileName.c_str(), mainDoc, jcheck_method);

    // finally, there really only needs to be one thing in mainDoc, and it
    // needs to be an interface.
    bool onlyParcelable = false;
    err |= exactly_one_interface(options.inputFileName.c_str(), mainDoc, options, &onlyParcelable);

    // If this includes an interface definition, then assign method ids and validate.
    if (!onlyParcelable) {
        err |= check_and_assign_method_ids(options.inputFileName.c_str(),
                ((interface_type*)mainDoc)->interface_items);
    }

    // after this, there shouldn't be any more errors because of the
    // input.
    if (err != 0 || mainDoc == NULL) {
        return 1;
    }

    // if needed, generate the outputFileName from the outputBaseFolder
    if (options.outputFileName.length() == 0 &&
            options.outputBaseFolder.length() > 0) {
        options.outputFileName = generate_outputFileName(options, mainDoc);
    }

    // if we were asked to, generate a make dependency file
    // unless it's a parcelable *and* it's supposed to fail on parcelable
    if ((options.autoDepFile || options.depFileName != "") &&
            !(onlyParcelable && options.failOnParcelable)) {
        // make sure the folders of the output file all exists
        check_outputFilePath(options.outputFileName);
        generate_dep_file(options, mainDoc);
    }

    // they didn't ask to fail on parcelables, so just exit quietly.
    if (onlyParcelable && !options.failOnParcelable) {
        return 0;
    }

    // make sure the folders of the output file all exists
    check_outputFilePath(options.outputFileName);

    err = generate_java(options.outputFileName, options.inputFileName.c_str(),
                        (interface_type*)mainDoc);

    return err;
}

