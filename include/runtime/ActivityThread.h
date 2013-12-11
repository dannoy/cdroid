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


class ActivityThread : public virtual RefBase{
public:
    ActivityThread();
    sp<Handler> getHandler();
    sp<ContextImpl> getSystemContext();
    void scheduleLaunchActivity(sp<ActivityClientRecord> r);

private:
    class H : public Handler {
    public:
        H(sp<ActivityThread> at)
            : mThread(at)
        {
        }
        enum {
            LAUNCH_ACTIVITY = 1,
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


private:
    sp<H> mH;
    bool mSystemThread;
    sp<ApplicationThread> mAppThread;;
    Vector<sp<ActivityClientRecord> > mActivities;


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
