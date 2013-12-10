#ifndef _SERVICE_ACTIVITY_MANAGER_SERVICE_H
#define _SERVICE_ACTIVITY_MANAGER_SERVICE_H

#include <service/IActivityManager.h>
#include <runtime/ActivityThread.h>
#include "ActivityStack.h"
#include "ProcessRecord.h"

namespace cdroid {

class ActivityManagerService : public ActivityManagerNative {
public:
    ActivityManagerService();
    void systemReady();

// IActivityManager interfaces
public:
    int attachApplication(sp<IApplicationThread> appThread);
    bool startHomeActivityLocked();
    sp<ProcessRecord> getRecordForAppLocked(sp<IApplicationThread> thread);
    sp<ProcessRecord> getProcessRecordLocked(String8 appName);
    sp<ProcessRecord> startProcessLocked(String8 appName);

private:

    pid_t MY_PID;
    sp<ActivityStack> mMainStack;
    String8 mTopAction;
    Vector<sp<ProcessRecord> > mProcesses;
    Vector<sp<ProcessRecord> > mProcessesPending;

public:
    static int main();
    static int setSystemProcess();
    static sp<ActivityManagerService> self();

    static sp<ActivityManagerService> mSelf;
    static sp<ActivityThread> mSystemThread;
};



};

#endif
