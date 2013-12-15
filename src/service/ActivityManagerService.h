#ifndef _SERVICE_ACTIVITY_MANAGER_SERVICE_H
#define _SERVICE_ACTIVITY_MANAGER_SERVICE_H

#include <service/IActivityManager.h>
#include <runtime/ActivityThread.h>
#include "ActivityStack.h"
#include "ProcessRecord.h"
#include "ActiveServices.h"
#include "ReceiverHub.h"

namespace cdroid {

class ActivityManagerService : public ActivityManagerNative {
public:
    ActivityManagerService();
    void systemReady();
    bool startHomeActivityLocked();
    sp<ProcessRecord> getRecordForAppLocked(sp<IApplicationThread> thread);
    sp<ProcessRecord> getProcessRecordLocked(String8 appName);
    sp<ProcessRecord> getProcessRecordLocked(sp<IApplicationThread> thread);
    sp<ProcessRecord> startProcessLocked(String8 appName);
    int startActivityLocked(sp<IApplicationThread> caller, sp<IBinder> resultTo, sp<Intent> intent, int pid, int uid, int requestCode);
    int startServiceLocked(sp<IApplicationThread> caller, sp<Intent> intent, int pid, int uid);
    int bindServiceLocked(sp<IApplicationThread> caller, sp<IBinder> token, sp<Intent> intent, sp<IServiceConnection> connection, int pid, int uid, int flags);
    int registerReceiverLocked(sp<IApplicationThread> caller, sp<IIntentReceiver> receiver, sp<IntentFilter> filter, int pid, int uid);
    int broadcastIntentLocked(sp<IApplicationThread> caller, sp<Intent> intent, sp<IIntentReceiver> resultTo, sp<Bundle> map, bool serialized, bool sticky, int pid, int uid);

// IActivityManager interfaces
public:
    virtual void attachApplication(sp<IBinder> appThread);
    virtual int startActivity(sp<IBinder> caller, sp<IBinder> resultTo, sp<Intent> intent, int requestCode);
    virtual int startService(sp<IBinder> caller, sp<Intent> intent) ;
    virtual int bindService(sp<IBinder> caller, sp<IBinder> token, sp<Intent> intent, sp<IBinder> connection, int flags);
    virtual int publishService(sp<IBinder> token, sp<Intent> intent, sp<IBinder> service);
    virtual int registerReceiver(sp<IBinder> caller, sp<IBinder> receiver, sp<IntentFilter> filter);
    virtual int broadcastIntent(sp<IBinder> caller, sp<Intent> intent, sp<IBinder> resultTo, sp<Bundle> map, bool serialized, bool sticky);

private:
    void attachApplicationLocked(sp<IBinder> appThread, pid_t callingPid);

public:
    sp<ActivityStack> mMainStack;
private:
    pid_t MY_PID;
    sp<ActiveServices> mServices;
    sp<ReceiverHub> mReceiverHub;
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
