#include <unistd.h>
#include "options.h"
#include <string.h>
#include "caidl.h"
#include "CType.h"
#include "aidl_language.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <map>

#ifdef HAVE_MS_C_RUNTIME
#include <io.h>
#endif

// The following are gotten as the offset from the allowable id's between
// android.os.IBinder.FIRST_CALL_TRANSACTION=1 and
// android.os.IBinder.LAST_CALL_TRANSACTION=16777215
#define MIN_USER_SET_METHOD_ID                0
#define MAX_USER_SET_METHOD_ID                16777214

static vector<string> g_includePaths;

// public functions



static int
cgather_types(const char* filename, document_item_type* items)
{
    int err = 0;
    while (items) {
        CType* type;
        if (items->item_type == USER_DATA_TYPE) {
            user_data_type* p = (user_data_type*)items;
            type = new CUserDataType(p->package ? package2namespace(p->package) : "", p->name.data,
                    false, ((p->flattening_methods & PARCELABLE_DATA) != 0),
                    ((p->flattening_methods & RPC_DATA) != 0), filename, p->name.lineno);
        }
        else if (items->item_type == INTERFACE_TYPE_BINDER
                || items->item_type == INTERFACE_TYPE_RPC) {
            interface_type* c = (interface_type*)items;
            type = new CInterfaceType(c->package ? package2namespace(c->package) : "",
                            c->name.data, false, c->oneway,
                            filename, c->name.lineno);
        }
        else {
            fprintf(stderr, "aidl: internal error %s:%d\n", __FILE__, __LINE__);
            return 1;
        }

        CType* old = CNAMES.Find(type->QualifiedName());
        if (old == NULL) {
            CNAMES.Add(type);

            if (items->item_type == INTERFACE_TYPE_BINDER) {
                // for interfaces, also add the BnInterface and BpInterface types, we don't
                // bother checking these for duplicates, because the parser
                // won't let us do it.
                interface_type* c = (interface_type*)items;

                vector<CType> args;
                args.push_back(type);
                CNAMES.AddTemplate(c->package ? package2namespace(c->package) : "",
                                        "BnInterface", args);
                CNAMES.AddTemplate(c->package ? package2namespace(c->package) : "",
                                        "BpInterface", args);

                string name = string("Bn") + c->name.data;
                Type* BnXXX = new Type(c->package ? package2namespace(c->package) : "",
                                        name, Type::GENERATED, false, false, false,
                                        filename, c->name.lineno);
                NAMES.Add(BnXXX);
                name = string("Bp") + c->name.data;
                Type* BpXXX = new Type(c->package ? package2namespace(c->package) : "",
                                        name, Type::GENERATED, false, false, false,
                                        filename, c->name.lineno);
                NAMES.Add(BpXXX);

            }
            else if (items->item_type == INTERFACE_TYPE_RPC) {
                fprintf(stderr, "%s:%d attempt to define RPC %s files which is not supported yet\n",
                            filename, type->DeclLine(),
                            type->QualifiedName().c_str());
            }
        } else {
            if (old->Kind() == CType::BUILT_IN) {
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
cmatches_keyword(const char* str)
{
    static const char* KEYWORDS[] = {
        "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case", "catch",
        "char", "class", "compl", "const", "const_cast", "continue", "default", "delete",
        "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false",
        "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable", "namespace",
        "new", "not", "not_eq", "operator", "or", "or_eq", "private", "protected", "public",
        "register", "reinterpret_cast", "return", "short", "signed", "sizeof", "static",
        "static_cast", "struct", "switch", "template", "this", "throw", "true", "try", "typedef",
        "typeid", "typename", "union", "unsigned", "using(1)", "virtual", "void", "volatile",
        "wchar_t", "while", "xor", "xor_eq","NULL",
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
ccheck_method(const char* filename, int kind, method_type* m)
{
    int err = 0;

    // return type
    CType* returnType = CNAMES.Search(m->type.type.data);
    if (returnType == NULL) {
        fprintf(stderr, "%s:%d unknown return type %s\n", filename,
                    m->type.type.lineno, m->type.type.data);
        err = 1;
        return err;
    }

    if (!(kind == INTERFACE_TYPE_BINDER ? returnType->CanWriteToParcel()
                : returnType->CanWriteToRpcData())) {
        fprintf(stderr, "%s:%d return type %s can't be marshalled.\n", filename,
                    m->type.type.lineno, m->type.type.data);
        err = 1;
    }

    if (m->type.dimension > 0) {
        fprintf(stderr, "%s:%d return type %s%s can't be an array.\n", filename,
                m->type.array_token.lineno, m->type.type.data,
                m->type.array_token.data);
        err = 1;
    }

    int index = 1;

    arg_type* arg = m->args;
    while (arg) {
        CType* t = CNAMES.Search(arg->type.type.data);

        // check the arg type
        if (t == NULL) {
            fprintf(stderr, "%s:%d parameter %s (%d) unknown type %s\n",
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

        if (arg->type.dimension > 0) {
            fprintf(stderr, "%s:%d parameter %d: '%s %s%s %s' can't be an"
                    " array.\n", filename,
                    m->type.array_token.lineno, index, arg->direction.data,
                    arg->type.type.data, arg->type.array_token.data,
                    arg->name.data);
            err = 1;
        }

        // check that the name doesn't match a keyword
        if (cmatches_keyword(arg->name.data)) {
            fprintf(stderr, "%s:%d parameter %d %s is named the same as a"
                    " C++ or aidl keyword\n",
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

enum {
    OUTPUT_FILE_HEADER,
    OUTPUT_FILE_SOURCE
};

// ==========================================================
static string
cgenerate_outputFileName2(const Options& options, const buffer_type& name,
                        const char* package, int type)
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
    if(OUTPUT_FILE_HEADER == type) {
        result += ".h";
    }
    else if(OUTPUT_FILE_SOURCE == type) {
        result += ".cpp";
    }

    return result;
}

// ==========================================================
static string
cgenerate_outputFileName(const Options& options, const document_item_type* items, int type)
{
    // items has already been checked to have only one interface.
    if (items->item_type == INTERFACE_TYPE_BINDER || items->item_type == INTERFACE_TYPE_RPC) {
        interface_type* type = (interface_type*)items;

        return generate_outputFileName2(options, type->name, type->package, type);
    } else if (items->item_type == USER_DATA_TYPE) {
        user_data_type* type = (user_data_type*)items;
        return generate_outputFileName2(options, type->name, type->package, int type);
    }

    // I don't think we can come here, but safer than returning NULL.
    string result;
    return result;
}





// ==========================================================
int
ccompile_aidl(Options& options)
{
    int err = 0, N;

    // options.importPaths must be include paths used for import statement in aidl
    set_import_paths(options.importPaths);

    cregister_base_types();

    // import the preprocessed file
    N = options.preprocessedFiles.size();
    for (int i=0; i<N; i++) {
        const string& s = options.preprocessedFiles[i];
        err |= parse_preprocessed_file(s, cgather_types);
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
        if (CNAMES.Find(import->neededClass) == NULL) {
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
        err |= cgather_types(import->filename, import->doc);
        import = import->next;
    }

    // check the referenced types in mainDoc to make sure we've imported them
    err |= check_types(options.inputFileName.c_str(), mainDoc, ccheck_method);

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

    string hf,sf;
    // if needed, generate the outputFileName from the outputBaseFolder
    if (options.outputFileName.length() == 0 &&
            options.outputBaseFolder.length() > 0) {
        //options.outputFileName = cgenerate_outputFileName(options, mainDoc);
        hf = cgenerate_outputFileName(options, mainDoc, OUTPUT_FILE_HEADER);
        sf = cgenerate_outputFileName(options, mainDoc, OUTPUT_FILE_SOURCE);
    }
    else {
        sf = options.outputFileName;
        hf = options.outputFileName;
        int n = hf.find('.');
        hf.replace(n + 1, 3, "h");
    }

    // if we were asked to, generate a make dependency file
    // unless it's a parcelable *and* it's supposed to fail on parcelable
    if ((options.autoDepFile || options.depFileName != "") &&
            !(onlyParcelable && options.failOnParcelable)) {
        // make sure the folders of the output file all exists
        //check_outputFilePath(options.outputFileName);
        check_outputFilePath(sf);
        generate_dep_file(options, mainDoc);
    }

    // they didn't ask to fail on parcelables, so just exit quietly.
    if (onlyParcelable && !options.failOnParcelable) {
        return 0;
    }

    // make sure the folders of the output file all exists
    check_outputFilePath(sf);

    err = cgenerate_cpp(hf, sf, options.inputFileName.c_str(),
                        (interface_type*)mainDoc);

    return err;
}
