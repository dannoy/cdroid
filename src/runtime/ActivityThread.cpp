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

int ActivityThread::callActivityOnCreate(sp<Activity> act)
{
    act->onCreate(NULL);
    return 0;
}

int ActivityThread::callActivityOnStart(sp<Activity> act)
{
    act->onStart();
    return 0;
}

int ActivityThread::callActivityOnResume(sp<Activity> act)
{
    act->onResume();
    return 0;
}

int ActivityThread::callActivityOnPause(sp<Activity> act)
{
    act->onPause();
    return 0;
}

int ActivityThread::callActivityOnStop(sp<Activity> act)
{
    act->onStop();
    return 0;
}

int ActivityThread::callActivityOnDestroy(sp<Activity> act)
{
    act->onDestroy();
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


void ActivityThread::ApplicationThread::schedulePauseActivity(sp<IBinder> token)
{
}

void ActivityThread::ApplicationThread::bindApplication(String8 appName)
{
    mAppName = appName;
    ALOGI("bindApplication %s",appName.string());
}

void ActivityThread::ApplicationThread::scheduleLaunchActivity(sp<ActivityInfo> ai, sp<IBinder> token, sp<Intent> intent)
{
    sp<ActivityClientRecord> r = new ActivityClientRecord;
    r->mActivityInfo = ai;
    r->mToken = token;
    r->mIntent = intent;
    sp<Message> msg = new Message(H::LAUNCH_ACTIVITY, r);
    mH->sendMessage(msg);
}

void ActivityThread::scheduleLaunchActivity(sp<ActivityClientRecord> r)
{
    sp<ActivityInfo> ai = r->mActivityInfo;
    //ALOGI("scheduleLaunchActivity phase II %s %p",ai->mName.string(), ai.get());

    ActivityManifest* am = ActivityLoader::loadActivity(ai->mFilename, ai->mName);
    if(!am) {
        ALOGI("cannot found activity: %s",ai->mName.string());
        return;
    }
    ALOGI("scheduleLaunchActivity %s found",am->name.string());
    sp<Activity> activity = am->createActivity(r->mIntent);

    if(activity == NULL) {
        ALOGI("scheduleLaunchActivity create %s failed!!",am->name.string());
        return;
    }

    r->mActivity = activity;

    sp<ContextImpl> appContext = new ContextImpl();

    appContext->init(am, r->mToken, this);
    appContext->setOuterContext(activity);

    activity->attach(appContext, this, r->mToken, r->mActivityInfo);


    mActivities.push_back(r);

    callActivityOnCreate(activity);
    callActivityOnStart(activity);
    callActivityOnResume(activity);
}

void ActivityThread::H::handleMessage(const sp<Message>& message)
{
    switch(message->what) {
        case LAUNCH_ACTIVITY:
            {
                sp<ActivityClientRecord> r = reinterpret_cast<ActivityClientRecord*>(message->obj.get());
                mThread->scheduleLaunchActivity(r);
            }
            break;
    }
}


};



