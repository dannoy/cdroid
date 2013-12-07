#ifndef _RUNTIME_PROCESS_H
#define _RUNTIME_PROCESS_H

#include <sys/types.h>
#include <unistd.h>

#include <runtime/common.h>

namespace cdroid {

class Process {
public:
    static int startViaZygote(String8& niceName, int uid, int gid, Vector<int>& gids, Vector<String8>& extraArgs);

    static pid_t myPid();
    static uid_t myUid();
    static uid_t myEUid();

private:
    static int startViaZygoteArgs(Vector<String8>& args);
};

};

#endif
