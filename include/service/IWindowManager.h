#ifndef _SERVICE_IWINDOWMANAGER_H
#define _SERVICE_IWINDOWMANAGER_H

#include <runtime/common.h>
#include <runtime/Looper.h>
#include <runtime/KeyEvent.h>
#include <runtime/Text.h>

namespace cdroid {

class IWindowCallback {
public:
    virtual void onKeyEvent(sp<KeyEvent> keyEvent) = 0;
};

class IWindow : public IWindowCallback, public IInterface {
public:
    DECLARE_META_INTERFACE(Window);
};

class BnWindow : public BnInterface<IWindow> {
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};


class IWindowManager : public IWindowCallback, public IInterface {
public:
    DECLARE_META_INTERFACE(WindowManager);
    virtual int registerFocusWindow(sp<IBinder> window) = 0;
    virtual int displayText(sp<Text> txt) = 0;
};

class BnWindowManager : public BnInterface<IWindowManager> {
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};

};

#endif
