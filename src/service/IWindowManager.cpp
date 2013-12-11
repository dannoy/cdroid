#define LOG_TAG "IWindowManager"
#include <cutils/log.h>

#include <service/IWindowManager.h>

namespace cdroid {
enum {
    TRANSACTION_windowOnKeyEvent = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
};

class BpWindow: public BpInterface<IWindow>
{
public:
    BpWindow(const sp<IBinder>& impl)
        : BpInterface<IWindow>(impl)
    {
    }
    virtual void onKeyEvent(sp<KeyEvent> keyEvent)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        keyEvent->writeToParcel(&_data, 0);
        remote()->transact(TRANSACTION_windowOnKeyEvent, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // ERROR
        }
    }
};

IMPLEMENT_META_INTERFACE(Window, "com::cdroid::service::IWindow");

int BnWindow::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_windowOnKeyEvent:
            {
                CHECK_INTERFACE(IWindow, data, reply);
                sp<KeyEvent> keyEvent;
                keyEvent = keyEvent->createFromParcel(data);
                onKeyEvent(keyEvent);
                reply->writeInt32(1);
                return true;
            }
            break;

    }

    return BBinder::onTransact(code, data, reply, flags);
}

enum {
    TRANSACTION_wmOnKeyEvent = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
    TRANSACTION_wmRegisterFocusWindow,
    TRANSACTION_wmDisplayText,
};

class BpWindowManager: public BpInterface<IWindowManager>
{
public:
    BpWindowManager(const sp<IBinder>& impl)
        : BpInterface<IWindowManager>(impl)
    {
    }

    virtual void onKeyEvent(sp<KeyEvent> keyEvent)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        keyEvent->writeToParcel(&_data, 0);
        remote()->transact(TRANSACTION_wmOnKeyEvent, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // ERROR
        }
    }
    virtual int registerFocusWindow(sp<IBinder> binder)
    {
        Parcel _data;
        Parcel _reply;

        int _result = -1;
        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(binder.get());
        remote()->transact(TRANSACTION_wmRegisterFocusWindow, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _reply.readInt32();
        }
        else {
            // ERROR
        }

        return _result;
    }
    virtual int displayText(sp<Text> txt)
    {
        Parcel _data;
        Parcel _reply;
        int _result = -1;
        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        txt->writeToParcel(&_data, 0);
        remote()->transact(TRANSACTION_wmDisplayText, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _reply.readInt32();
        }
        else {
            // ERROR
        }

        return _result;
    }
};

IMPLEMENT_META_INTERFACE(WindowManager, "com::cdroid::service::IWindowManager");
int BnWindowManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_wmOnKeyEvent:
            {
                CHECK_INTERFACE(IWindowManager, data, reply);
                sp<KeyEvent> keyEvent;
                keyEvent = keyEvent->createFromParcel(data);
                onKeyEvent(keyEvent);
                reply->writeInt32(1);
                return true;
            }
            break;

        case TRANSACTION_wmRegisterFocusWindow:
            {
                CHECK_INTERFACE(IWindowManager, data, reply);
                sp<IBinder> binder = data.readStrongBinder();
                int result = registerFocusWindow(binder);
                reply->writeInt32(1);
                reply->writeInt32(result);
                return true;
            }
            break;
        case TRANSACTION_wmDisplayText:
            {
                CHECK_INTERFACE(IWindowManager, data, reply);
                sp<Text> txt;
                txt = txt->createFromParcel(data);
                int result = displayText(txt);
                reply->writeInt32(1);
                reply->writeInt32(result);
                return true;
            }
            break;

    }
    return BBinder::onTransact(code, data, reply, flags);
}

};

