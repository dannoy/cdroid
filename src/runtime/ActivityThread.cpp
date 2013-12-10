#define LOG_TAG "ActivityThread"
#include <cutils/log.h>

#include <runtime/ActivityThread.h>
#include <runtime/Process.h>
#include <service/IActivityManager.h>

namespace cdroid {

sp<Handler> ActivityThread::sMainThreadHandler;
sp<ActivityThread> ActivityThread::sCurrentActivityThread;
sp<ContextImpl> ActivityThread::sSystemContext;

ActivityThread::ActivityThread()
{
    mH = new H;
    mAppThread = new ApplicationThread;
}


int ActivityThread::main(Vector<String8>& args)
{
    Looper::prepareMainLooper();

    sp<ActivityThread> thread = new ActivityThread;

    sCurrentActivityThread = thread;

    sMainThreadHandler = thread->getHandler();

    thread->attach(false);

    Looper::loop();
}

sp<ActivityThread> ActivityThread::systemMain()
{
    sp<ActivityThread> thread = new ActivityThread;
    thread->attach(true);

    return thread;
}

sp<Handler> ActivityThread::getHandler()
{
    return mH;
}

int ActivityThread::attach(bool system)
{
    mSystemThread = system;
    // If system is true, activitymanagerservice hasn't been ready
    if(!system) {
        sp<IActivityManager> mgr = ActivityManagerNative::getDefault();
        mgr->attachApplication(mAppThread);
    }

    return 0;
}

sp<Handler> ActivityThread::getMainHandler()
{
    return sMainThreadHandler;
}

sp<ActivityThread> ActivityThread::getCurrentActivityThread()
{
    return sCurrentActivityThread;
}

sp<ContextImpl> ActivityThread::getSystemContext()
{
    if(sSystemContext == NULL) {
        ContextImpl *context = ContextImpl::createSystemContext(this);

        sSystemContext = context;
    }
    return sSystemContext;
}

void ActivityThread::H::handleMessage(const Message& message)
{
}


void ActivityThread::ApplicationThread::schedulePauseActivity(sp<IBinder> token)
{
}

void ActivityThread::ApplicationThread::bindApplication(String8 appName)
{
}


};



