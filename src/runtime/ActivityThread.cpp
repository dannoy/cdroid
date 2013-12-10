#define LOG_TAG "ActivityThread"
#include <cutils/log.h>

#include <runtime/ActivityThread.h>
#include <runtime/Process.h>
#include <runtime/ApplicationLoader.h>
#include <service/IActivityManager.h>

namespace cdroid {

sp<Handler> ActivityThread::sMainThreadHandler;
sp<ActivityThread> ActivityThread::sCurrentActivityThread;
sp<ContextImpl> ActivityThread::sSystemContext;

ActivityThread::ActivityThread()
{
    mH = new H(this);
    mAppThread = new ApplicationThread(mH);
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

void ActivityThread::H::handleMessage(const sp<Message>& message)
{
    switch(message->what) {
        case LAUNCH_ACTIVITY:
            {
    ALOGI("H::handleMessage launchactivity %p ",message->obj.get());
                sp<ActivityInfo> ai = reinterpret_cast<ActivityInfo*>(message->obj.get());
    ALOGI("H::handleMessage launchactivity ai %p %p",ai.get(),mThread.get());
                mThread->scheduleLaunchActivity(ai);
    ALOGI("H::handleMessage launchactivity ai %p over",ai.get());
            }
            break;
    }
}


void ActivityThread::ApplicationThread::schedulePauseActivity(sp<IBinder> token)
{
}

void ActivityThread::ApplicationThread::bindApplication(String8 appName)
{
    mAppName = appName;
    ALOGI("bindApplication %s",appName.string());
}

void ActivityThread::ApplicationThread::scheduleLaunchActivity(sp<ActivityInfo> ai)
{
    sp<Message> msg = new Message(H::LAUNCH_ACTIVITY, ai);
    ai->incStrong(this);
    ai->incStrong(this);
    ALOGI("scheduleLaunchActivity sendmessage %p",msg->obj.get());

    mH->sendMessage(msg);
}

void ActivityThread::scheduleLaunchActivity(sp<ActivityInfo> ai)
{
    ALOGI("scheduleLaunchActivity phase II %s %p",ai->mName.string(), ai.get());

    ActivityManifest* am = ActivityLoader::loadActivity(ai->mFilename, ai->mName);
    if(!am) {
        ALOGI("cannot found activity: %s",ai->mName.string());
        return;
    }
    ALOGI("scheduleLaunchActivity %s found",am->name.string());
}

};



