#define LOG_TAG "WindowManagerService"
#include <cutils/log.h>

#include <service/DisplayManager.h>
#include "WindowManagerService.h"
#include "PackageManagerService.h"

namespace cdroid {
class WThread : public Thread {
public:
    sp<Looper> mLooper;
    Condition *mCond;
    WThread(Condition* cond)
        :Thread(false)
    {
        mCond = cond;
    }
    virtual ~WThread()
    {
    }

    virtual bool threadLoop(){
        Looper::prepare();

        mLooper = Looper::myLooper();
        //ALOGE("ActivityManagerService threadloop %d %p", Process::myPid(), mLooper.get());
        mCond->signal();

        Looper::loop();

        return true;
    }
};


WindowManagerService::WindowManagerService()
{
    Condition* cond = new Condition;
    Mutex mutex;
    WThread* thr = new WThread(cond);
    thr->run();

    while(thr->mLooper.get() == NULL) {
        cond->wait(mutex);
    };
    mLooper = thr->mLooper;
    mH = new H(this, mLooper);
}

void WindowManagerService::instantiate()
{
    android::defaultServiceManager()->addService(android::String16("window"), new WindowManagerService());
}

int WindowManagerService::registerFocusWindow(sp<IBinder> window)
{
    //ALOGI("registerFocusWindow");
    mFocusWindow = interface_cast<IWindow>(window);
    return 0;
}

void WindowManagerService::onKeyEvent(sp<KeyEvent> keyEvent)
{
    //ALOGI("keyEvent %x", keyEvent->getCode());
    sp<Message> msg = new Message(H::ON_KEYEVENT, keyEvent);
    mH->sendMessage(msg);
}

int WindowManagerService::displayText(sp<Text> txt)
{
    sp<Message> msg = new Message(H::ON_DISPLAY_TEXT, txt);
    mH->sendMessage(msg);

    return 0;
}

void WindowManagerService::handleOnKeyEventLocked(sp<KeyEvent> keyEvent)
{
    //ALOGI("handleOnkeyEvent %x", keyEvent->getCode());

    if(mFocusWindow != NULL) {
        mFocusWindow->onKeyEvent(keyEvent);
    }
}

void WindowManagerService::handleDisplayTextLocked(sp<Text> txt)
{
    //ALOGI("handleDisplayText %s", txt->getCharSequence());
    DisplayManager disp;
    disp.displayText(txt);
}

void WindowManagerService::H::handleMessage(const sp<Message>& message)
{
    switch(message->what) {
        case ON_KEYEVENT:
            {
                sp<KeyEvent> e = reinterpret_cast<KeyEvent*>(message->obj.get());
                mWMS->handleOnKeyEventLocked(e);
            }
            break;
        case ON_DISPLAY_TEXT:
            {
                sp<Text> txt = reinterpret_cast<Text*>(message->obj.get());
                mWMS->handleDisplayTextLocked(txt);
            }
            break;
    }
}

};
