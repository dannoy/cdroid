#ifndef _SERVICE_WINDOWMANAGER_SERVICE_H
#define _SERVICE_WINDOWMANAGER_SERVICE_H

#include <runtime/common.h>
#include <runtime/Looper.h>
#include <service/IWindowManager.h>

namespace cdroid {

class WindowManagerService : public BnWindowManager {
public:
    WindowManagerService();
    static void instantiate();
    void handleOnKeyEventLocked(sp<KeyEvent> keyEvent);
    void handleDisplayTextLocked(sp<Text> txt);

    // interfaces
    virtual void onKeyEvent(sp<KeyEvent> keyEvent);
    virtual int registerFocusWindow(sp<IBinder> binder);
    virtual int displayText(sp<Text> txt);

private:
    class H : public Handler {
    public:
        H(sp<WindowManagerService> wms, sp<Looper> looper)
            : mWMS(wms),
              Handler(looper)
        {
        }
        enum {
            ON_KEYEVENT = 1,
            ON_DISPLAY_TEXT,
        };
        virtual void handleMessage(const sp<Message>& message);
    private:
        sp<WindowManagerService> mWMS;
    };

private:
    sp<IWindow> mFocusWindow;
    sp<Looper> mLooper;
    sp<Handler> mH;

    Mutex mMutex;
};

};

#endif
