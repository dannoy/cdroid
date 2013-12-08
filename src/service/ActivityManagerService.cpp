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
    AThread(sp<Condition> cond)
        :Thread(false)
    {
    }
    virtual ~AThread()
    {
    }

    virtual bool threadLoop(){
        Looper::prepare();

        mLooper = Looper::myLooper();

        cond->signal();


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
    mSelf = ActivityManagerServiceSingleton::Instance();
    mSystemThread = ActivityThread::systemMain();
    sp<Context> = mSystemThread->getSystemContext();
    while(thr->mLooper == NULL) {
        cond->wait(mutex);
    };
    mMainStack = new ActivityStack(mSelf, context, true, thr->mLooper);
}

int ActivityManagerService::setSystemProcess()
{
    android::defaultServiceManager()->addService(android::String16("activity"), mSelf.get());

    return 0;
}

void ActivityManagerService::systemReady()
{
    ALOGI("System now ready");
    mMainStack->resumeTopActivityLocked(NULL);

    return 0;
}

};
