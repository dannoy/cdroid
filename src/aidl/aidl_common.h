#ifndef _AIDL_COMMON_H_
#define _AIDL_COMMON_H_

#include <string>
#include <vector>

#include "options.h"

#ifdef HAVE_MS_C_RUNTIME
#include <io.h>
#include <sys/stat.h>
#endif

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

#endif
