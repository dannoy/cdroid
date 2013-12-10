#ifndef _SERVICE_PROCESS_RECORD_H
#define _SERVICE_PROCESS_RECORD_H

#include <runtime/common.h>
#include <runtime/Process.h>

namespace cdroid {

class ProcessRecord : public RefBase {
public:
    sp<IApplicationThread> thread;
    String8 name;
    pid_t pid;
    uid_t uid;
};

};

#endif
