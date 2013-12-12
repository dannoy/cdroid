#define LOG_TAG "Activity"
#include <cutils/log.h>

#include <runtime/Activity.h>
#include <runtime/ActivityThread.h>


namespace cdroid {


Activity::Activity(sp<Intent> intent)
{
    mIntent = intent;
}


void Activity::onCreate(sp<Bundle> savedInstanceState)
{
    ALOGI("onCreate");
}

void Activity::onStart()
{
    ALOGI("onStart");
}

void Activity::onResume()
{
    ALOGI("onResume");
    WindowManager wm;
    wm.registerFocusWindow(mWindow);
}

void Activity::onPause()
{
    ALOGI("onPause");
}

void Activity::onStop()
{
    ALOGI("onStop");
}

void Activity::onDestroy()
{
    ALOGI("onDestroy");
}

int Activity::attach(sp<Context> context, sp<ActivityThread> thread, sp<IBinder> token, sp<ActivityInfo> ai)
{
    attachBaseContext(context);
    mThread = thread;
    mToken = token;
    mActivityInfo = ai;
    mWindow = new ActivityWindow(this);
}

void Activity::onKeyEvent(sp<KeyEvent> keyEvent)
{
    //ALOGI("onKeyEvent %x", keyEvent->getCode());

    char str[2];
    str[0] = keyEvent->getCode();
    str[1] = '\0';
    sp<Text> txt = new Text(str);

    if(str[0] == '\n') {
        execInternalCommand(mCmd);
        mCmd.clear();
    } else {
        mCmd.append(str);
    }

    WindowManager wm;
    wm.displayText(txt);
}

Activity::ActivityWindow::ActivityWindow(sp<Activity> activity)
{
    mActivity = activity;
}

void Activity::ActivityWindow::onKeyEvent(sp<KeyEvent> keyEvent)
{
    mActivity->onKeyEvent(keyEvent);
}

};


