#define LOG_TAG "ActivityThread"
#include <cutils/log.h>

#include <runtime/ActivityThread.h>
#include <runtime/Process.h>
#include <runtime/ApplicationLoader.h>
#include <service/IActivityManager.h>

namespace cdroid {

class IBinderMessageObject : public RefBase {
public:
    IBinderMessageObject(sp<IBinder> binder)
        : mBinder(binder)
    {
    }

    sp<IBinder> mBinder;
};

class CMDThread : public Thread {
public:
    sp<Looper> mLooper;
    Condition *mCond;
    CMDThread(Condition* cond)
        :Thread(false)
    {
        mCond = cond;
    }
    virtual ~CMDThread()
    {
    }

    virtual bool threadLoop(){
        Looper::prepare();

        mLooper = Looper::myLooper();
        //ALOGE("Activity threadloop %d %p", Process::myPid(), mLooper.get());
        mCond->signal();

        Looper::loop();

        return true;
    }
};

sp<Handler> ActivityThread::sMainThreadHandler;
sp<ActivityThread> ActivityThread::sCurrentActivityThread;
sp<ContextImpl> ActivityThread::sSystemContext;

ActivityThread::ActivityThread()
{

    Condition* cond = new Condition;
    Mutex mutex;
    CMDThread* thr = new CMDThread(cond);
    thr->run();

    while(thr->mLooper.get() == NULL) {
        cond->wait(mutex);
    };

    mCmdLooper = thr->mLooper;

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

int ActivityThread::callServiceOnCreate(sp<Service> srv)
{
    srv->onCreate();
    return 0;
}

int ActivityThread::callServiceOnStart(sp<Service> srv)
{
    srv->onStart();
    return 0;
}

int ActivityThread::callServiceOnStartCommand(sp<Service> srv, sp<Intent> intent)
{
    srv->onStartCommand(intent);
    return 0;
}

sp<IBinder> ActivityThread::callServiceOnBind(sp<Service> srv, sp<Intent> intent)
{
    return srv->onBind(intent);
}

int ActivityThread::callServiceOnUnBind(sp<Service> srv)
{
    srv->onUnBind();
    return 0;
}

int ActivityThread::callServiceOnDestroy(sp<Service> srv)
{
    srv->onDestroy();
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
sp<IBinder> ActivityThread::getApplicationThread()
{
    return mAppThread;
}


void ActivityThread::ApplicationThread::schedulePauseActivity(sp<IBinder> token)
{
    sp<IBinderMessageObject> obj = new IBinderMessageObject(token);
    sp<Message> msg = new Message(H::PAUSE_ACTIVITY, obj);
    mH->sendMessage(msg);
}

void ActivityThread::ApplicationThread::bindApplication(String8 appName)
{
    mAppName = appName;
    ALOGI("bindApplication %s",appName.string());
}

void ActivityThread::ApplicationThread::scheduleLaunchActivity(sp<ActivityInfo> ai, sp<IBinder> token, sp<Intent> intent)
{
    //ALOGI("scheduleLaunchActivity");
    sp<ActivityClientRecord> r = new ActivityClientRecord;
    r->mActivityInfo = ai;
    r->mToken = token;
    r->mIntent = intent;
    sp<Message> msg = new Message(H::LAUNCH_ACTIVITY, r);
    mH->sendMessage(msg);
    //ALOGI("scheduleLaunchActivity sent over");
}

void ActivityThread::ApplicationThread::scheduleCreateService(sp<ServiceInfo> ai, sp<IBinder> token, sp<Intent> intent)
{
    //ALOGI("scheduleCreateService");
    sp<ServiceClientRecord> r = new ServiceClientRecord;
    r->mServiceInfo = ai;
    r->mToken = token;
    r->mIntent = intent;
    sp<Message> msg = new Message(H::CREATE_SERVICE, r);
    mH->sendMessage(msg);
}

void ActivityThread::ApplicationThread::scheduleBindService(sp<IBinder> token, sp<Intent> intent, bool rebind)
{
    //ALOGI("scheduleBindService");
    sp<BindServiceData> r = new BindServiceData(token, intent, rebind);
    sp<Message> msg = new Message(H::BIND_SERVICE, r);
    mH->sendMessage(msg);
}

void ActivityThread::ApplicationThread::scheduleReceiver(sp<Intent> intent, sp<Bundle> bundle, bool ordered, bool sticky)
{
    //ALOGI("scheduleReceiver");
    sp<ScheduleReceiverData> data = new ScheduleReceiverData(intent, bundle, ordered, sticky);
    sp<Message> msg = new Message(H::SCHEDULE_RECEIVER, data);
    mH->sendMessage(msg);
}

void ActivityThread::scheduleLaunchActivity(sp<ActivityClientRecord> r)
{
    AutoMutex _l(mMutex);
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

    appContext->init(am, r->mToken, this, mCmdLooper);
    appContext->setOuterContext(activity);

    activity->attach(appContext, this, r->mToken, r->mActivityInfo);


    mActivities.push_back(r);

    callActivityOnCreate(activity);
    callActivityOnStart(activity);
    callActivityOnResume(activity);
}

void ActivityThread::schedulePauseActivity(sp<IBinder> token)
{
    AutoMutex _l(mMutex);
    sp<ActivityClientRecord> r;
    if(token != NULL) {
        for(Vector<sp<ActivityClientRecord> >::iterator it = mActivities.begin(); it != mActivities.end(); ++it) {
            if((*it)->mToken == token) {
                r = *it;
            }
        }
    }
    if(r!=NULL) {
        ALOGI("schedulePauseActivity %s", r->mActivityInfo->getName().string());
        callActivityOnPause(r->mActivity);
    }
    else {
        ALOGI("schedulePauseActivity %p not found ", token.get());
    }
}

void ActivityThread::scheduleCreateService(sp<ServiceClientRecord> r)
{
    AutoMutex _l(mMutex);

    sp<ServiceInfo> si = r->mServiceInfo;
    //ALOGI("scheduleCreateService phase II %s %p",si->mName.string(), si.get());

    ServiceManifest* sMF = ServiceLoader::loadService(si->mFilename, si->mName);
    if(!sMF) {
        ALOGI("cannot found activity: %s",si->mName.string());
        return;
    }
    ALOGI("scheduleCreateServcie %s found",sMF->name.string());
    sp<Service> service = sMF->createService(r->mIntent);

    if(service == NULL) {
        ALOGI("scheduleLaunchActivity create %s failed!!",sMF->name.string());
        return;
    }

    r->mService = service;

    sp<ContextImpl> appContext = new ContextImpl();

    appContext->init(sMF, r->mToken, this);
    appContext->setOuterContext(service);

    service->attach(appContext, this, r->mToken, r->mServiceInfo);


    mServices.push_back(r);

    callServiceOnCreate(service);
    callServiceOnStart(service);
    callServiceOnStartCommand(service, r->mIntent);
}

sp<ServiceClientRecord> ActivityThread::getServiceRecordByToken(sp<IBinder> token)
{
    Vector<sp<ServiceClientRecord> >::iterator it = mServices.begin();

    for(; it != mServices.end(); ++it) {
        if((*it)->mToken == token) {
            return *it;
        }
    }

    return NULL;
}

void ActivityThread::scheduleBindService(sp<BindServiceData> d)
{
    AutoMutex _l(mMutex);
    ALOGE("bindservice ");
    sp<ServiceClientRecord> r = getServiceRecordByToken(d->mToken);
    if(r == NULL) {
        ALOGE("ERROR:r == NULL while bindservice");
        return;
    }
    sp<IBinder> binder = r->mService->onBind(d->mIntent);
    ActivityManagerNative::getDefault()->publishService(d->mToken, d->mIntent, binder);
}

void ActivityThread::scheduleReceiver(sp<ScheduleReceiverData> d)
{
    AutoMutex _l(mMutex);
    ALOGE("scheduleReceiver ");
}

void ActivityThread::H::handleMessage(const sp<Message>& message)
{
    //ALOGI("handlemessage");
    switch(message->what) {
        case LAUNCH_ACTIVITY:
            {
                sp<ActivityClientRecord> r = reinterpret_cast<ActivityClientRecord*>(message->obj.get());
                mThread->scheduleLaunchActivity(r);
            }
            break;
        case PAUSE_ACTIVITY:
            {
                sp<IBinderMessageObject> obj = reinterpret_cast<IBinderMessageObject*>(message->obj.get());
                sp<IBinder> token = obj->mBinder;
                mThread->schedulePauseActivity(token);
            }
            break;
        case CREATE_SERVICE:
            {
                sp<ServiceClientRecord> obj = reinterpret_cast<ServiceClientRecord*>(message->obj.get());
                mThread->scheduleCreateService(obj);
            }
            break;
        case BIND_SERVICE:
            {
                sp<BindServiceData> obj = reinterpret_cast<BindServiceData*>(message->obj.get());
                mThread->scheduleBindService(obj);
            }
            break;
        case SCHEDULE_RECEIVER:
            {
                sp<ScheduleReceiverData> obj = reinterpret_cast<ScheduleReceiverData*>(message->obj.get());
                mThread->scheduleReceiver(obj);
            }
            break;
    }
}


};



