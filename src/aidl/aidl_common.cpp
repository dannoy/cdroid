
#include "aidl_common.h"
#include "Type.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static vector<string> g_importPaths;

void
set_import_paths(const vector<string>& importPaths)
{
    g_importPaths = importPaths;
}

string package2namespace(string package)
{
    int i;
    while((i = package.find('.')) != package.npos) {
        package.replace(i,1,"::");
    }
    return package;
}


char*
find_import_file(const char* given)
{
    string expected = given;

    int N = expected.length();
    for (int i=0; i<N; i++) {
        char c = expected[i];
        if (c == '.') {
            expected[i] = OS_PATH_SEPARATOR;
        }
    }
    expected += ".aidl";

    vector<string>& paths = g_importPaths;
    for (vector<string>::iterator it=paths.begin(); it!=paths.end(); it++) {
        string f = *it;
        if (f.size() == 0) {
            f = ".";
            f += OS_PATH_SEPARATOR;
        }
        else if (f[f.size()-1] != OS_PATH_SEPARATOR) {
            f += OS_PATH_SEPARATOR;
        }
        f.append(expected);

#ifdef HAVE_MS_C_RUNTIME
        /* check that the file exists and is not write-only */
        if (0 == _access(f.c_str(), 0) &&  /* mode 0=exist */
            0 == _access(f.c_str(), 4) ) { /* mode 4=readable */
#else
        if (0 == access(f.c_str(), R_OK)) {
#endif        
            return strdup(f.c_str());
        }
    }

    return NULL;
}

void
test_document(document_item_type* d)
{
    while (d) {
        if (d->item_type == INTERFACE_TYPE_BINDER) {
            interface_type* c = (interface_type*)d;
            printf("interface %s %s {\n", c->package, c->name.data);
            interface_item_type *q = (interface_item_type*)c->interface_items;
            while (q) {
                if (q->item_type == METHOD_TYPE) {
                    method_type *m = (method_type*)q;
                    printf("  %s %s(", m->type.type.data, m->name.data);
                    arg_type *p = m->args;
                    while (p) {
                        printf("%s %s",p->type.type.data,p->name.data);
                        if (p->next) printf(", ");
                        p=p->next;
                    }
                    printf(")");
                    printf(";\n");
                }
                q=q->next;
            }
            printf("}\n");
        }
        else if (d->item_type == USER_DATA_TYPE) {
            user_data_type* b = (user_data_type*)d;
            if ((b->flattening_methods & PARCELABLE_DATA) != 0) {
                printf("parcelable %s %s;\n", b->package, b->name.data);
            }
            if ((b->flattening_methods & RPC_DATA) != 0) {
                printf("flattenable %s %s;\n", b->package, b->name.data);
            }
        }
        else {
            printf("UNKNOWN d=0x%08lx d->item_type=%d\n", (long)d, d->item_type);
        }
        d = d->next;
    }
}

// ==========================================================
int
convert_direction(const char* direction)
{
    if (direction == NULL) {
        return IN_PARAMETER;
    }
    if (0 == strcmp(direction, "in")) {
        return IN_PARAMETER;
    }
    if (0 == strcmp(direction, "out")) {
        return OUT_PARAMETER;
    }
    return INOUT_PARAMETER;
}

// ==========================================================

document_item_type* g_document = NULL;
import_info* g_imports = NULL;

static void
main_document_parsed(document_item_type* d)
{
    g_document = d;
}

char*
parse_import_statement(const char* text)
{
    const char* end;
    int len;

    while (isspace(*text)) {
        text++;
    }
    while (!isspace(*text)) {
        text++;
    }
    while (isspace(*text)) {
        text++;
    }
    end = text;
    while (!isspace(*end) && *end != ';') {
        end++;
    }
    len = end-text;

    char* rv = (char*)malloc(len+1);
    memcpy(rv, text, len);
    rv[len] = '\0';

    return rv;
}

static void
main_import_parsed(buffer_type* statement)
{
    import_info* import = (import_info*)malloc(sizeof(import_info));
    memset(import, 0, sizeof(import_info));
    import->from = strdup(g_currentFilename);
    import->statement.lineno = statement->lineno;
    import->statement.data = strdup(statement->data);
    import->statement.extra = NULL;
    import->next = g_imports;
    import->neededClass = parse_import_statement(statement->data);
    g_imports = import;
}

ParserCallbacks g_mainCallbacks = {
    &main_document_parsed,
    &main_import_parsed
};

// ==========================================================
static void
import_import_parsed(buffer_type* statement)
{
}

static ParserCallbacks g_importCallbacks = {
    &main_document_parsed,
    &import_import_parsed
};

// ==========================================================
static int
check_filename(const char* filename, const char* package, buffer_type* name)
{
    const char* p;
    string expected;
    string fn;
    size_t len;
    char cwd[MAXPATHLEN];
    bool valid = false;

#ifdef HAVE_WINDOWS_PATHS
    if (isalpha(filename[0]) && filename[1] == ':'
        && filename[2] == OS_PATH_SEPARATOR) {
#else
    if (filename[0] == OS_PATH_SEPARATOR) {
#endif
        fn = filename;
    } else {
        fn = getcwd(cwd, sizeof(cwd));
        len = fn.length();
        if (fn[len-1] != OS_PATH_SEPARATOR) {
            fn += OS_PATH_SEPARATOR;
        }
        fn += filename;
    }

    if (package) {
        expected = package;
        expected += '.';
    }

    len = expected.length();
    for (size_t i=0; i<len; i++) {
        if (expected[i] == '.') {
            expected[i] = OS_PATH_SEPARATOR;
        }
    }

    p = strchr(name->data, '.');
    len = p ? p-name->data : strlen(name->data);
    expected.append(name->data, len);
    
    expected += ".aidl";

    len = fn.length();
    valid = (len >= expected.length());

    if (valid) {
        p = fn.c_str() + (len - expected.length());

#ifdef HAVE_WINDOWS_PATHS
        if (OS_PATH_SEPARATOR != '/') {
            // Input filename under cygwin most likely has / separators
            // whereas the expected string uses \\ separators. Adjust
            // them accordingly.
          for (char *c = const_cast<char *>(p); *c; ++c) {
                if (*c == '/') *c = OS_PATH_SEPARATOR;
            }
        }
#endif

#ifdef OS_CASE_SENSITIVE
        valid = (expected == p);
#else
        valid = !strcasecmp(expected.c_str(), p);
#endif
    }

    if (!valid) {
        fprintf(stderr, "%s:%d interface %s should be declared in a file"
                " called %s.\n",
                filename, name->lineno, name->data, expected.c_str());
        return 1;
    }

    return 0;
}

int
check_filenames(const char* filename, document_item_type* items)
{
    int err = 0;
    while (items) {
        if (items->item_type == USER_DATA_TYPE) {
            user_data_type* p = (user_data_type*)items;
            err |= check_filename(filename, p->package, &p->name);
        }
        else if (items->item_type == INTERFACE_TYPE_BINDER
                || items->item_type == INTERFACE_TYPE_RPC) {
            interface_type* c = (interface_type*)items;
            err |= check_filename(filename, c->package, &c->name);
        }
        else {
            fprintf(stderr, "aidl: internal error unkown document type %d.\n",
                        items->item_type);
            return 1;
        }
        items = items->next;
    }
    return err;
}

// ==========================================================
const char*
kind_to_string(int kind)
{
    switch (kind)
    {
        case Type::INTERFACE:
            return "an interface";
        case Type::USERDATA:
            return "a user data";
        default:
            return "ERROR";
    }
}

char*
rfind(char* str, char c)
{
    char* p = str + strlen(str) - 1;
    while (p >= str) {
        if (*p == c) {
            return p;
        }
        p--;
    }
    return NULL;
}

// ==========================================================
int
parse_preprocessed_file(const string& filename, int (*gather_types)(const char* , document_item_type* ))
{
    int err;

    FILE* f = fopen(filename.c_str(), "rb");
    if (f == NULL) {
        fprintf(stderr, "aidl: can't open preprocessed file: %s\n",
                filename.c_str());
        return 1;
    }

    int lineno = 1;
    char line[1024];
    char type[1024];
    char fullname[1024];
    while (fgets(line, sizeof(line), f)) {
        // skip comments and empty lines
        if (!line[0] || strncmp(line, "//", 2) == 0) {
          continue;
        }

        sscanf(line, "%s %[^; \r\n\t];", type, fullname);

        char* packagename;
        char* classname = rfind(fullname, '.');
        if (classname != NULL) {
            *classname = '\0';
            classname++;
            packagename = fullname;
        } else {
            classname = fullname;
            packagename = NULL;
        }

        //printf("%s:%d:...%s...%s...%s...\n", filename.c_str(), lineno,
        //        type, packagename, classname);
        document_item_type* doc;
        
        if (0 == strcmp("parcelable", type)) {
            user_data_type* parcl = (user_data_type*)malloc(
                    sizeof(user_data_type));
            memset(parcl, 0, sizeof(user_data_type));
            parcl->document_item.item_type = USER_DATA_TYPE;
            parcl->keyword_token.lineno = lineno;
            parcl->keyword_token.data = strdup(type);
            parcl->package = packagename ? strdup(packagename) : NULL;
            parcl->name.lineno = lineno;
            parcl->name.data = strdup(classname);
            parcl->semicolon_token.lineno = lineno;
            parcl->semicolon_token.data = strdup(";");
            parcl->flattening_methods = PARCELABLE_DATA;
            doc = (document_item_type*)parcl;
        }
        else if (0 == strcmp("flattenable", type)) {
            user_data_type* parcl = (user_data_type*)malloc(
                    sizeof(user_data_type));
            memset(parcl, 0, sizeof(user_data_type));
            parcl->document_item.item_type = USER_DATA_TYPE;
            parcl->keyword_token.lineno = lineno;
            parcl->keyword_token.data = strdup(type);
            parcl->package = packagename ? strdup(packagename) : NULL;
            parcl->name.lineno = lineno;
            parcl->name.data = strdup(classname);
            parcl->semicolon_token.lineno = lineno;
            parcl->semicolon_token.data = strdup(";");
            parcl->flattening_methods = RPC_DATA;
            doc = (document_item_type*)parcl;
        }
        else if (0 == strcmp("interface", type)) {
            interface_type* iface = (interface_type*)malloc(
                    sizeof(interface_type));
            memset(iface, 0, sizeof(interface_type));
            iface->document_item.item_type = INTERFACE_TYPE_BINDER;
            iface->interface_token.lineno = lineno;
            iface->interface_token.data = strdup(type);
            iface->package = packagename ? strdup(packagename) : NULL;
            iface->name.lineno = lineno;
            iface->name.data = strdup(classname);
            iface->open_brace_token.lineno = lineno;
            iface->open_brace_token.data = strdup("{");
            iface->close_brace_token.lineno = lineno;
            iface->close_brace_token.data = strdup("}");
            doc = (document_item_type*)iface;
        }
        else {
            fprintf(stderr, "%s:%d: bad type in line: %s\n",
                    filename.c_str(), lineno, line);
            return 1;
        }
        err = gather_types(filename.c_str(), doc);
        lineno++;
    }

    if (!feof(f)) {
        fprintf(stderr, "%s:%d: error reading file, line to long.\n",
                filename.c_str(), lineno);
        return 1;
    }

    fclose(f);
    return 0;
}

// ==========================================================
int
exactly_one_interface(const char* filename, const document_item_type* items, const Options& options,
                      bool* onlyParcelable)
{
    if (items == NULL) {
        fprintf(stderr, "%s: file does not contain any interfaces\n",
                            filename);
        return 1;
    }

    const document_item_type* next = items->next;
    // Allow parcelables to skip the "one-only" rule.
    if (items->next != NULL && next->item_type != USER_DATA_TYPE) {
        int lineno = -1;
        if (next->item_type == INTERFACE_TYPE_BINDER) {
            lineno = ((interface_type*)next)->interface_token.lineno;
        }
        else if (next->item_type == INTERFACE_TYPE_RPC) {
            lineno = ((interface_type*)next)->interface_token.lineno;
        }
        fprintf(stderr, "%s:%d aidl can only handle one interface per file\n",
                            filename, lineno);
        return 1;
    }

    if (items->item_type == USER_DATA_TYPE) {
        *onlyParcelable = true;
        if (options.failOnParcelable) {
            fprintf(stderr, "%s:%d aidl can only generate code for interfaces, not"
                            " parcelables or flattenables,\n", filename,
                            ((user_data_type*)items)->keyword_token.lineno);
            fprintf(stderr, "%s:%d .aidl files that only declare parcelables or flattenables"
                            "may not go in the Makefile.\n", filename,
                            ((user_data_type*)items)->keyword_token.lineno);
            return 1;
        }
    } else {
        *onlyParcelable = false;
    }

    return 0;
}

int
check_and_assign_method_ids(const char * filename, interface_item_type* first_item)
{
    // Check whether there are any methods with manually assigned id's and any that are not.
    // Either all method id's must be manually assigned or all of them must not.
    // Also, check for duplicates of user set id's and that the id's are within the proper bounds.
    set<int> usedIds;
    interface_item_type* item = first_item;
    bool hasUnassignedIds = false;
    bool hasAssignedIds = false;
    while (item != NULL) {
        if (item->item_type == METHOD_TYPE) {
            method_type* method_item = (method_type*)item;
            if (method_item->hasId) {
                hasAssignedIds = true;
                method_item->assigned_id = atoi(method_item->id.data);
                // Ensure that the user set id is not duplicated.
                if (usedIds.find(method_item->assigned_id) != usedIds.end()) {
                    // We found a duplicate id, so throw an error.
                    fprintf(stderr,
                            "%s:%d Found duplicate method id (%d) for method: %s\n",
                            filename, method_item->id.lineno,
                            method_item->assigned_id, method_item->name.data);
                    return 1;
                }
                // Ensure that the user set id is within the appropriate limits
                if (method_item->assigned_id < MIN_USER_SET_METHOD_ID ||
                        method_item->assigned_id > MAX_USER_SET_METHOD_ID) {
                    fprintf(stderr, "%s:%d Found out of bounds id (%d) for method: %s\n",
                            filename, method_item->id.lineno,
                            method_item->assigned_id, method_item->name.data);
                    fprintf(stderr, "    Value for id must be between %d and %d inclusive.\n",
                            MIN_USER_SET_METHOD_ID, MAX_USER_SET_METHOD_ID);
                    return 1;
                }
                usedIds.insert(method_item->assigned_id);
            } else {
                hasUnassignedIds = true;
            }
            if (hasAssignedIds && hasUnassignedIds) {
                fprintf(stderr,
                        "%s: You must either assign id's to all methods or to none of them.\n",
                        filename);
                return 1;
            }
        }
        item = item->next;
    }

    // In the case that all methods have unassigned id's, set a unique id for them.
    if (hasUnassignedIds) {
        int newId = 0;
        item = first_item;
        while (item != NULL) {
            if (item->item_type == METHOD_TYPE) {
                method_type* method_item = (method_type*)item;
                method_item->assigned_id = newId++;
            }
            item = item->next;
        }
    }

    // success
    return 0;
}

int
check_types(const char* filename, document_item_type* items,
                    int (*check_method)(const char* , int , method_type* ))
{
    int err = 0;
    while (items) {
        // (nothing to check for USER_DATA_TYPE)
        if (items->item_type == INTERFACE_TYPE_BINDER
                || items->item_type == INTERFACE_TYPE_RPC) {
            map<string,method_type*> methodNames;
            interface_type* c = (interface_type*)items;

            interface_item_type* member = c->interface_items;
            while (member) {
                if (member->item_type == METHOD_TYPE) {
                    method_type* m = (method_type*)member;

                    err |= check_method(filename, items->item_type, m);

                    // prevent duplicate methods
                    if (methodNames.find(m->name.data) == methodNames.end()) {
                        methodNames[m->name.data] = m;
                    } else {
                        fprintf(stderr,"%s:%d attempt to redefine method %s,\n",
                                filename, m->name.lineno, m->name.data);
                        method_type* old = methodNames[m->name.data];
                        fprintf(stderr, "%s:%d    previously defined here.\n",
                                filename, old->name.lineno);
                        err = 1;
                    }
                }
                member = member->next;
            }
        }

        items = items->next;
    }
    return err;
}

// ==========================================================
void
check_outputFilePath(const string& path) {
    size_t len = path.length();
    for (size_t i=0; i<len ; i++) {
        if (path[i] == OS_PATH_SEPARATOR) {
            string p = path.substr(0, i);
            if (access(path.data(), F_OK) != 0) {
#ifdef HAVE_MS_C_RUNTIME
                _mkdir(p.data());
#else
                mkdir(p.data(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP);
#endif
            }
        }
    }
}

int
preprocess_aidl(const Options& options)
{
    vector<string> lines;
    int err;

    // read files
    int N = options.filesToPreprocess.size();
    for (int i=0; i<N; i++) {
        g_callbacks = &g_mainCallbacks;
        err = parse_aidl(options.filesToPreprocess[i].c_str());
        if (err != 0) {
            return err;
        }
        document_item_type* doc = g_document;
        string line;
        if (doc->item_type == USER_DATA_TYPE) {
            user_data_type* parcelable = (user_data_type*)doc;
            if ((parcelable->flattening_methods & PARCELABLE_DATA) != 0) {
                line = "parcelable ";
            }
            if ((parcelable->flattening_methods & RPC_DATA) != 0) {
                line = "flattenable ";
            }
            if (parcelable->package) {
                line += parcelable->package;
                line += '.';
            }
            line += parcelable->name.data;
        } else {
            line = "interface ";
            interface_type* iface = (interface_type*)doc;
            if (iface->package) {
                line += iface->package;
                line += '.';
            }
            line += iface->name.data;
        }
        line += ";\n";
        lines.push_back(line);
    }

    // write preprocessed file
    int fd = open( options.outputFileName.c_str(), 
                   O_RDWR|O_CREAT|O_TRUNC|O_BINARY,
#ifdef HAVE_MS_C_RUNTIME
                   _S_IREAD|_S_IWRITE);
#else    
                   S_IRUSR|S_IWUSR|S_IRGRP);
#endif            
    if (fd == -1) {
        fprintf(stderr, "aidl: could not open file for write: %s\n",
                options.outputFileName.c_str());
        return 1;
    }

    N = lines.size();
    for (int i=0; i<N; i++) {
        const string& s = lines[i];
        int len = s.length();
        if (len != write(fd, s.c_str(), len)) {
            fprintf(stderr, "aidl: error writing to file %s\n",
                options.outputFileName.c_str());
            close(fd);
            unlink(options.outputFileName.c_str());
            return 1;
        }
    }

    close(fd);
    return 0;
}

string
gather_comments(extra_text_type* extra)
{
    string s;
    while (extra) {
        if (extra->which == SHORT_COMMENT) {
            s += extra->data;
        }
        else if (extra->which == LONG_COMMENT) {
            s += "/*";
            s += extra->data;
            s += "*/";
        }
        extra = extra->next;
    }
    return s;
}

string
append(const char* a, const char* b)
{
    string s = a;
    s += b;
    return s;
}

