#define LOG_TAG "ActivityThread"
#include <cutils/log.h>

#include <runtime/ActivityThread.h>
#include <service/IActivityManager.h>

namespace cdroid {

sp<Handler> ActivityThread::sMainThreadHandler;
sp<ActivityThread> ActivityThread::sCurrentActivityThread;

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
    sp<IActivityManager> mgr = ActivityManagerNative::getDefault();
    mgr->attachApplication(mAppThread);

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

void ActivityThread::H::handleMessage(const Message& message)
{
}



};



