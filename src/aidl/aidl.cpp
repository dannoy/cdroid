
#include "aidl_language.h"
#include "options.h"
#include "caidl.h"
#include "jaidl.h"
#include "generate_java.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

#ifdef HAVE_MS_C_RUNTIME
#include <io.h>
#include <sys/stat.h>
#endif

#ifndef O_BINARY
#  define O_BINARY  0
#endif


using namespace std;

// ==========================================================
static int
compile_aidl(Options& options)
{
    int err = 0;

    if(options.lang == TARGET_CPP) {
        err = ccompile_aidl(options);
    }
    else if(options.lang == TARGET_JAVA) {
        err = ccompile_aidl(options);
    }

    return err;
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

// ==========================================================
int
main(int argc, const char **argv)
{
    Options options;
    int result = parse_options(argc, argv, &options);
    if (result) {
        return result;
    }

    switch (options.task)
    {
        case COMPILE_AIDL:
            return compile_aidl(options);
        case PREPROCESS_AIDL:
            return preprocess_aidl(options);
    }
    fprintf(stderr, "aidl: internal error\n");
    return 1;
}
