#define LOG_TAG "ActivityThread"
#include <cutils/log.h>

#include <runtime/ActivityThread.h>

namespace cdroid {

sp<Handler> ActivityThread::sMainThreadHandler;

ActivityThread::ActivityThread()
{
    mH = new H;
}


int ActivityThread::main(Vector<String8>& args)
{
    Looper::prepareMainLooper();

    sp<ActivityThread> thread = new ActivityThread;

    sMainThreadHandler = thread->getHandler();

    thread->attach(false);

    Looper::loop();
}

sp<Handler> ActivityThread::getHandler()
{
    return mH;
}

int ActivityThread::attach(bool system)
{

    mSystemThread = system;
}

sp<Handler> ActivityThread::getMainHandler()
{
    return sMainThreadHandler;
}

void ActivityThread::H::handleMessage(const Message& message)
{
}



};



