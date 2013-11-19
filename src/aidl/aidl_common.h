#ifndef _AIDL_COMMON_H_
#define _AIDL_COMMON_H_

#include <string>
#include <vector>
#include <map>

#include "options.h"
#include "aidl_language.h"

#ifdef HAVE_MS_C_RUNTIME
#include <io.h>
#include <sys/stat.h>
#endif

#ifndef O_BINARY
#  define O_BINARY  0
#endif

// The following are gotten as the offset from the allowable id's between
// android.os.IBinder.FIRST_CALL_TRANSACTION=1 and
// android.os.IBinder.LAST_CALL_TRANSACTION=16777215
#define MIN_USER_SET_METHOD_ID                0
#define MAX_USER_SET_METHOD_ID                16777214

struct import_info {
    const char* from;
    const char* filename;
    buffer_type statement;
    const char* neededClass;
    document_item_type* doc;
    struct import_info* next;
};

extern void set_import_paths(const vector<string>& importPaths);
extern string package2namespace(string package);
extern char* find_import_file(const char* given);
extern int convert_direction(const char* direction);
extern int check_filenames(const char* filename, document_item_type* items);
extern const char* kind_to_string(int kind);
extern char* rfind(char* str, char c);
extern int parse_preprocessed_file(const string& filename, int (*gather_types)(const char* , document_item_type* ));
extern int exactly_one_interface(const char* filename, const document_item_type* items, const Options& options,
                                                bool* onlyParcelable);
extern int check_and_assign_method_ids(const char * filename, interface_item_type* first_item);
extern int check_types(const char* filename, document_item_type* items,
                        int (*check_method)(const char* , int , method_type* ));
extern void check_outputFilePath(const string& path);
extern int preprocess_aidl(const Options& options);

extern document_item_type* g_document;
extern import_info* g_imports;
extern ParserCallbacks g_mainCallbacks;

extern string gather_comments(extra_text_type* extra);
extern string append(const char* a, const char* b);

#endif
