#ifndef _RUNTIME_PROCESS_H
#define _RUNTIME_PROCESS_H

#include <runtime/common.h>

namespace cdroid {

class Process {
public:
    static int startViaZygote(String8& niceName, int uid, int gid, Vector<int>& gids, Vector<String8>& extraArgs);

private:
    static int startViaZygoteArgs(Vector<String8>& args);
};

};

#endif
