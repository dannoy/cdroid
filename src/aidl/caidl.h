#ifndef _CAIDL_H_
#define _CAIDL_H_

#include "options.h"

int ccompile_aidl(Options& options);
int cpreprocess_aidl(const Options& options);
char* cfind_include_file(const char* given);
void cset_include_paths(const vector<string>& includePaths);

#endif
