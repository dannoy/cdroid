#ifndef _RUNTIME_ACTIVITY_H
#define _RUNTIME_ACTIVITY_H

#include <runtime/Context.h>
#include <runtime/Bundle.h>
#include <runtime/Intent.h>
#include <runtime/ActivityInfo.h>
#include <service/IWindowManager.h>
#include <service/WindowManager.h>


namespace cdroid {

class ActivityThread;

class Activity : public ContextWrapper, public IWindowCallback
{
public:
    Activity(sp<Intent> intent);
    // life cycle callback
    virtual void onCreate(sp<Bundle> savedInstanceState);
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
    // event callback
    virtual void onKeyEvent(sp<KeyEvent> keyEvent);

public:
    int attach(sp<Context> context, sp<ActivityThread> thread, sp<IBinder> token, sp<ActivityInfo> ai);

private:
    class ActivityWindow : public BnWindow {
    public:
        ActivityWindow(sp<Activity> activity);
        virtual void onKeyEvent(sp<KeyEvent> keyEvent);
    private:
        sp<Activity> mActivity;
    };



private:
    sp<Intent> mIntent;
    sp<ActivityThread> mThread;
    sp<IBinder> mToken;
    sp<ActivityInfo> mActivityInfo;
    sp<ActivityWindow> mWindow;
    String8 mCmd;

};

};


#endif
