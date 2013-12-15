#ifndef _RUNTIME_ACTIVITY_THREAD_H
#define _RUNTIME_ACTIVITY_THREAD_H

#include <runtime/common.h>
#include <runtime/Looper.h>
#include <runtime/IApplicationThread.h>

#include <runtime/ContextImpl.h>

namespace cdroid {

class ActivityClientRecord : public RefBase {
public:
    sp<ActivityInfo> mActivityInfo;
    sp<IBinder> mToken;
    sp<Intent> mIntent;
    sp<Activity> mActivity;
};

class ServiceClientRecord : public RefBase {
public:
    sp<ServiceInfo> mServiceInfo;
    sp<IBinder> mToken;
    sp<Intent> mIntent;
    sp<Service> mService;
};

class BindServiceData : public RefBase {
public:
    BindServiceData(sp<IBinder> token, sp<Intent> intent, bool rebind)
        : mToken(token),
        mIntent(intent),
        mRebind(rebind)
    {
    }
    sp<IBinder> mToken;
    sp<Intent> mIntent;
    bool      mRebind;
};

class ScheduleReceiverData : public RefBase {
public:
    ScheduleReceiverData(sp<Intent> intent, sp<Bundle> bundle, bool ordered, bool sticky)
        : mIntent(intent),
        mBundle(bundle),
        mOrdered(ordered),
        mSticky(sticky)
    {
    }
    sp<Intent> mIntent;
    sp<Bundle> mBundle;
    bool mOrdered;
    bool mSticky;
};

class ActivityThread : public virtual RefBase{
public:
    ActivityThread();
    sp<Handler> getHandler();
    sp<ContextImpl> getSystemContext();
    sp<IBinder> getApplicationThread();
    void scheduleLaunchActivity(sp<ActivityClientRecord> r);
    void schedulePauseActivity(sp<IBinder> token);
    void scheduleCreateService(sp<ServiceClientRecord> r);
    void scheduleBindService(sp<BindServiceData> d);
    void scheduleReceiver(sp<ScheduleReceiverData> d);

private:
    class H : public Handler {
    public:
        H(sp<ActivityThread> at)
            : mThread(at),
            Handler()
        {
        }
        enum {
            LAUNCH_ACTIVITY = 1,
            PAUSE_ACTIVITY,
            CREATE_SERVICE,
            BIND_SERVICE,
            SCHEDULE_RECEIVER,
        };
        virtual void handleMessage(const sp<Message>& message);
    private:
        sp<ActivityThread> mThread;
    };

    class ApplicationThread : public BnApplicationThread {
    public:
        ApplicationThread(sp<Handler> h)
            : mH(h)
        {
        }
        virtual void schedulePauseActivity(sp<IBinder> token);
        virtual void bindApplication(String8 appName);
        virtual void scheduleLaunchActivity(sp<ActivityInfo> ai, sp<IBinder> token, sp<Intent> intent);
        virtual void scheduleCreateService(sp<ServiceInfo> ai, sp<IBinder> token, sp<Intent> intent);
        virtual void scheduleBindService(sp<IBinder> token, sp<Intent> intent, bool rebind);
        virtual void scheduleReceiver(sp<Intent> intent, sp<Bundle> bundle, bool ordered, bool sticky);
    private:
        String8 mAppName;
        sp<Handler> mH;
    };

    int attach(bool system);

    int callActivityOnCreate(sp<Activity> act);
    int callActivityOnStart(sp<Activity> act);
    int callActivityOnResume(sp<Activity> act);
    int callActivityOnPause(sp<Activity> act);
    int callActivityOnStop(sp<Activity> act);
    int callActivityOnDestroy(sp<Activity> act);

    int callServiceOnCreate(sp<Service> srv);
    int callServiceOnStart(sp<Service> srv);
    int callServiceOnStartCommand(sp<Service> srv, sp<Intent> intent);
    sp<IBinder> callServiceOnBind(sp<Service> srv, sp<Intent> intent);
    int callServiceOnUnBind(sp<Service> srv);
    int callServiceOnDestroy(sp<Service> srv);

    sp<ServiceClientRecord> getServiceRecordByToken(sp<IBinder> token);
private:
    sp<H> mH;
    bool mSystemThread;
    sp<ApplicationThread> mAppThread;;
    Vector<sp<ActivityClientRecord> > mActivities;
    Vector<sp<ServiceClientRecord> > mServices;

    sp<Looper> mCmdLooper;

    Mutex mMutex;


// Static
public:
    static int main(Vector<String8>& args);
    static sp<Handler> getMainHandler();
    static sp<ActivityThread> getCurrentActivityThread();
    static sp<ActivityThread> systemMain();
    private:
    static sp<Handler> sMainThreadHandler;
    static sp<ActivityThread> sCurrentActivityThread;
    static sp<ContextImpl> sSystemContext;

};

};


#endif
