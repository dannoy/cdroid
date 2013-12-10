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
    for(Vector<sp<ProcessRecord> >::iterator it = mProcessesPending.begin(); it != mProcesses.end(); ++it) {
        if((*it)->pid == callingPid) {
            app = *it;
            mProcessesPending.erase(it);
        }
    }

    if(app == NULL) {
        ALOGE("Omit unknown attach request from pid %d, killing", callingPid);
        Process::killProcessByPid(callingPid);
        return;
    }

    app->thread = interface_cast<IApplicationThread>(appThread);

    if(app->thread != NULL) {
        app->thread->bindApplication(app->name);
    }

    mMainStack->realStartActivityLocked(NULL, app);
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

    return NULL;
}

};
