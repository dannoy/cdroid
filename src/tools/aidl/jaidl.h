#ifndef _JAIDL_H_
#define _JAIDL_H_

#include "options.h"
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

int jcompile_aidl(Options& options);
int jpreprocess_aidl(const Options& options);

// returns a FILE* and the char* for the file that it found
// given is the class name we're looking for
char* jfind_import_file(const char* given);
void jset_import_paths(const vector<string>& importPaths);

#endif
