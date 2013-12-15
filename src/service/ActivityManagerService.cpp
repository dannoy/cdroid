#define LOG_TAG "ActivityManagerService"
#include <cutils/log.h>
#include <binder/IServiceManager.h>

#include <service/IActivityManager.h>
#include <runtime/Process.h>
#include <runtime/Intent.h>
#include "ActivityManagerService.h"


namespace cdroid {
class AThread : public Thread {
public:
    sp<Looper> mLooper;
    Condition *mCond;
    AThread(Condition* cond)
        :Thread(false)
    {
        mCond = cond;
    }
    virtual ~AThread()
    {
    }

    virtual bool threadLoop(){
        Looper::prepare();

        mLooper = Looper::myLooper();
        //ALOGE("ActivityManagerService threadloop %d %p", Process::myPid(), mLooper.get());

        mCond->signal();


        Looper::loop();

        return true;
    }
};

typedef Singleton<ActivityManagerService> ActivityManagerServiceSingleton;


sp<ActivityManagerService> ActivityManagerService::mSelf;
sp<ActivityThread> ActivityManagerService::mSystemThread;

ActivityManagerService::ActivityManagerService()
{
    MY_PID = Process::myPid();
}

void ActivityManagerService::attachApplication(sp<IBinder> appThread)
{
    AutoMutex _l(mMutex);

    int pid = IPCThreadState::self()->getCallingPid();
    ALOGI("attach request from pid %d %p", pid, appThread.get());
    int64_t origId = IPCThreadState::self()->clearCallingIdentity(); 
    attachApplicationLocked(appThread, pid);
    IPCThreadState::self()->restoreCallingIdentity(origId); 
}

void ActivityManagerService::attachApplicationLocked(sp<IBinder> appThread, int callingPid)
{
    sp<ProcessRecord> app;
    for(Vector<sp<ProcessRecord> >::iterator it = mProcessesPending.begin(); it != mProcessesPending.end(); ++it) {
        if((*it)->pid == callingPid) {
            app = *it;
            mProcessesPending.erase(it);
            break;
        }
    }

    if(app == NULL) {
        ALOGE("Omit unknown attach request from pid %d, killing", callingPid);
        Process::killProcessByPid(callingPid);
        return;
    }

    //ALOGI("Found attach request from pid %d app %s", callingPid, app->name.string());

    app->thread = interface_cast<IApplicationThread>(appThread);

    //ALOGI("Found attach request from pid %d app %s %p", callingPid, app->name.string(), app->thread.get());

    if(app->thread != NULL) {
        app->thread->bindApplication(app->name);
    }
    //ALOGI("Found attach request from pid %d app %s %p 222222222222", callingPid, app->name.string(), app->thread.get());

    //ALOGI("Found attach request from pid %d app %s", callingPid, app->name.string());
    int res1 = 0, res2 = 0, res3 = 0;
    res1 = mMainStack->attachApplicationLocked(app);
    //ALOGI("Found attach request from pid %d app %s: after mainstack attach", callingPid, app->name.string());

    res2 = mServices->attachApplicationLocked(app);
    //ALOGI("Found attach request from pid %d app %s: after service attach", callingPid, app->name.string());

    res3 = mReceiverHub->attachApplicationLocked(app);
    if(res1 == -1 && res2 == -1 && res3 == -1) {
        Process::killProcessByPid(app->pid);
    }
}

int ActivityManagerService::startActivity(sp<IBinder> caller, sp<IBinder> resultTo, sp<Intent> intent, int code)
{
    //ALOGI("startActivity %s", intent->getAction().string());
    int pid = IPCThreadState::self()->getCallingPid();
    int uid = IPCThreadState::self()->getCallingUid();
    sp<IApplicationThread> appThread = interface_cast<IApplicationThread>(caller);
    //ALOGI("startActivity %s request from pid %d %p", intent->getAction().string(), pid, caller.get());
    int64_t origId = IPCThreadState::self()->clearCallingIdentity(); 
    startActivityLocked(appThread, resultTo, intent, pid, uid, code);
    IPCThreadState::self()->restoreCallingIdentity(origId); 
}

int ActivityManagerService::startActivityLocked(sp<IApplicationThread> caller, sp<IBinder> resultTo, sp<Intent> intent, int pid, int uid, int requestCode)
{
    AutoMutex _l(mMutex);
    ALOGI("startActivity %s", intent->getAction().string());
    mMainStack->startActivityLocked(caller, intent, resultTo, pid, uid, requestCode);
}

int ActivityManagerService::startService(sp<IBinder> caller, sp<Intent> intent)
{
    //ALOGI("startService %s", intent->getAction().string());
    int pid = IPCThreadState::self()->getCallingPid();
    int uid = IPCThreadState::self()->getCallingUid();
    sp<IApplicationThread> appThread = interface_cast<IApplicationThread>(caller);
    int64_t origId = IPCThreadState::self()->clearCallingIdentity(); 
    startServiceLocked(appThread, intent, pid, uid);
    IPCThreadState::self()->restoreCallingIdentity(origId); 
}

int ActivityManagerService::startServiceLocked(sp<IApplicationThread> caller, sp<Intent> intent, int pid, int uid)
{
    AutoMutex _l(mMutex);
    ALOGI("startService %s", intent->getAction().string());
    mServices->startServiceLocked(caller, intent, pid, uid);
}
int ActivityManagerService::bindService(sp<IBinder> caller, sp<IBinder> token, sp<Intent> intent, sp<IBinder> connection, int flags)
{
    int pid = IPCThreadState::self()->getCallingPid();
    int uid = IPCThreadState::self()->getCallingUid();
    sp<IApplicationThread> appThread = interface_cast<IApplicationThread>(caller);
    sp<IServiceConnection> conn = interface_cast<IServiceConnection>(connection);
    int64_t origId = IPCThreadState::self()->clearCallingIdentity(); 
    bindServiceLocked(appThread, token, intent, conn, pid, uid, flags);
    IPCThreadState::self()->restoreCallingIdentity(origId); 
}

int ActivityManagerService::bindServiceLocked(sp<IApplicationThread> caller, sp<IBinder> token, sp<Intent> intent, sp<IServiceConnection> connection, int pid, int uid, int flags)
{
    AutoMutex _l(mMutex);
    ALOGI("bindService %s", intent->getAction().string());
    mServices->bindServiceLocked(caller, token, intent, connection, pid, uid, flags);
}

int ActivityManagerService::publishService(sp<IBinder> token, sp<Intent> intent, sp<IBinder> service)
{
    mServices->publishServiceLocked(token, intent, service);
}

int ActivityManagerService::registerReceiver(sp<IBinder> caller, sp<IBinder> receiver, sp<IntentFilter> filter)
{
    int pid = IPCThreadState::self()->getCallingPid();
    int uid = IPCThreadState::self()->getCallingUid();
    sp<IApplicationThread> appThread = interface_cast<IApplicationThread>(caller);
    sp<IIntentReceiver> r = interface_cast<IIntentReceiver>(receiver);
    int64_t origId = IPCThreadState::self()->clearCallingIdentity(); 
    registerReceiverLocked(appThread, r, filter, pid, uid);
    IPCThreadState::self()->restoreCallingIdentity(origId); 

}

int ActivityManagerService::registerReceiverLocked(sp<IApplicationThread> caller, sp<IIntentReceiver> receiver, sp<IntentFilter> filter, int pid, int uid)
{
    ALOGI("registerReceiverLocked ");
    return mReceiverHub->registerReceiverLocked(caller, receiver, filter, pid, uid);
}

int ActivityManagerService::broadcastIntent(sp<IBinder> caller, sp<Intent> intent, sp<IBinder> resultTo, sp<Bundle> map, bool serialized, bool sticky)
{
    int pid = IPCThreadState::self()->getCallingPid();
    int uid = IPCThreadState::self()->getCallingUid();
    sp<IApplicationThread> appThread = interface_cast<IApplicationThread>(caller);
    sp<IIntentReceiver> r = interface_cast<IIntentReceiver>(resultTo);
    int64_t origId = IPCThreadState::self()->clearCallingIdentity(); 
    broadcastIntentLocked(appThread, intent, r, map, serialized, sticky, pid, uid);
    IPCThreadState::self()->restoreCallingIdentity(origId); 
}

int ActivityManagerService::broadcastIntentLocked(sp<IApplicationThread> caller, sp<Intent> intent, sp<IIntentReceiver> resultTo, sp<Bundle> map, bool serialized, bool sticky, int pid, int uid)
{
    ALOGI("broadcastIntentLocked ");
    return mReceiverHub->broadcastIntentLocked(caller, intent, resultTo, map, serialized, sticky, pid, uid);
}

int ActivityManagerService::main()
{
    Condition* cond = new Condition;
    Mutex mutex;
    AThread* thr = new AThread(cond);;
    thr->run();
    mSelf = ActivityManagerServiceSingleton::Instance();
    mSystemThread = ActivityThread::systemMain();
    sp<Context> context= mSystemThread->getSystemContext();
    while(thr->mLooper.get() == NULL) {
        cond->wait(mutex);
    };
    mSelf->mMainStack = new ActivityStack(mSelf, context, true, thr->mLooper);

    mSelf->mServices = new ActiveServices(mSelf);
    mSelf->mReceiverHub = new ReceiverHub(mSelf);

    mSelf->mTopAction = Intent::ACTION_MAIN;
}

int ActivityManagerService::setSystemProcess()
{
    android::defaultServiceManager()->addService(android::String16("activity"), mSelf.get());

    return 0;
}

sp<ActivityManagerService> ActivityManagerService::self()
{
    return mSelf;
}

void ActivityManagerService::systemReady()
{
    ALOGI("System now ready");
    mMainStack->resumeTopActivityLocked(NULL);
}

bool ActivityManagerService::startHomeActivityLocked()
{
    sp<Intent> intent = new Intent(mTopAction);

    mMainStack->startActivityLocked(NULL, intent, NULL, 0, 0, 0);
}

sp<ProcessRecord> ActivityManagerService::getRecordForAppLocked(sp<IApplicationThread> thread)
{
    for(Vector<sp<ProcessRecord> >::iterator it = mProcesses.begin(); it != mProcesses.end(); ++it) {
        if((*it)->thread == thread) {
            return *it;
        }
    }

    return NULL;
}

sp<ProcessRecord> ActivityManagerService::getProcessRecordLocked(sp<IApplicationThread> thread)
{
    for(Vector<sp<ProcessRecord> >::iterator it = mProcesses.begin(); it != mProcesses.end(); ++it) {
        if((*it)->thread->asBinder() == thread->asBinder()) {
            return *it;
        }
    }

    return NULL;
}

sp<ProcessRecord> ActivityManagerService::getProcessRecordLocked(String8 appName)
{
    for(Vector<sp<ProcessRecord> >::iterator it = mProcesses.begin(); it != mProcesses.end(); ++it) {
        if((*it)->name == appName) {
            return *it;
        }
    }

    return NULL;
}

sp<ProcessRecord> ActivityManagerService::startProcessLocked(String8 appName)
{
    sp<ProcessRecord> app = getProcessRecordLocked(appName);

    if(app == NULL) {
        app = new ProcessRecord;
        app->name = appName;
        mProcesses.push_back(app);

        Vector<int> gids;
        Vector<String8> args;
        int ret = Process::startViaZygote(appName, 0, 0, gids, args, &app->pid);
        if(0 == ret) {
            mProcessesPending.push_back(app);
            ALOGI("Process [%s] started, pid %d",appName.string(), app->pid);
        }
    }

    return app;
}

};
