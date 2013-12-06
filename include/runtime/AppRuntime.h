#ifndef _RUNTIME_APPTUNTIME_H
#define _RUNTIME_APPTUNTIME_H

#include <runtime/common.h>

namespace cdroid {

class AppRuntime {
public:
    static int main(Vector<String8>& args, int argc, char *argv[]);
    static int onStart();

};

};

#endif
