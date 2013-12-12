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
    bool startHomeActivityLocked();
    sp<ProcessRecord> getRecordForAppLocked(sp<IApplicationThread> thread);
    sp<ProcessRecord> getProcessRecordLocked(String8 appName);
    sp<ProcessRecord> startProcessLocked(String8 appName);
    int startActivityLocked(sp<IApplicationThread> caller, sp<IBinder> resultTo, sp<Intent> intent, int pid, int uid, int requestCode);

// IActivityManager interfaces
public:
    virtual void attachApplication(sp<IBinder> appThread);
    virtual int startActivity(sp<IBinder> caller, sp<IBinder> resultTo, sp<Intent> intent, int requestCode);
    virtual int startService(sp<IBinder> caller, sp<Intent> intent) ;

private:
    void attachApplicationLocked(sp<IBinder> appThread, pid_t callingPid);

private:
    pid_t MY_PID;
    sp<ActivityStack> mMainStack;
    String8 mTopAction;
    Vector<sp<ProcessRecord> > mProcesses;
    Vector<sp<ProcessRecord> > mProcessesPending;

    Mutex mMutex;

public:
    static int main();
    static int setSystemProcess();
    static sp<ActivityManagerService> self();

    static sp<ActivityManagerService> mSelf;
    static sp<ActivityThread> mSystemThread;
};



};

#endif
