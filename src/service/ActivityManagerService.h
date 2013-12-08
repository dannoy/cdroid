#ifndef _SERVICE_ACTIVITY_MANAGER_SERVICE_H
#define _SERVICE_ACTIVITY_MANAGER_SERVICE_H

#include <service/IActivityManager.h>
#include <runtime/ActivityThread.h>
#include "ActivityStack.h"

namespace cdroid {

class ActivityManagerService : public ActivityManagerNative {
public:
    ActivityManagerService();
    void systemReady();

// IActivityManager interfaces
public:
    int attachApplication(sp<IApplicationThread> appThread);
private:

    pid_t MY_PID;
    sp<ActivityStack> mMainStack;

public:
    static int main();
    static int setSystemProcess();

    static sp<ActivityManagerService> mSelf;
    static sp<ActivityThread> mSystemThread;
};



};

#endif
