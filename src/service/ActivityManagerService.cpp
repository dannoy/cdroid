#define LOG_TAG "ActivityManagerService"
#include <cutils/log.h>
#include <binder/IServiceManager.h>

#include <service/IActivityManager.h>
#include <runtime/Process.h>
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
    ALOGE("ActivityManagerService threadloop %d %p", Process::myPid(), mLooper.get());

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

int ActivityManagerService::attachApplication(sp<IApplicationThread> appThread)
{
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
    //ALOGE("ActivityManagerService main5 %d", Process::myPid());
    mSelf->mMainStack = new ActivityStack(mSelf, context, true, thr->mLooper);
    //ALOGE("ActivityManagerService main6 %d", Process::myPid());
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

};
