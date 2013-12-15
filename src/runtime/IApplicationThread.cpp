
#include <runtime/IApplicationThread.h>

namespace cdroid {
enum {
    TRANSACTION_schedulePauseActivity = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
    TRANSACTION_bindApplication,
    TRANSACTION_scheduleLaunchActivity,
    TRANSACTION_scheduleCreateService,
    TRANSACTION_scheduleBindService,
    TRANSACTION_scheduleReceiver,
};

class BpApplicationThread : public BpInterface<IApplicationThread> {
public:
    BpApplicationThread(const sp<IBinder>& impl)
        : BpInterface<IApplicationThread>(impl)
    {
    }
    virtual void schedulePauseActivity(sp<IBinder> token)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(token);
        remote()->transact(TRANSACTION_schedulePauseActivity, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // Something is wrong
        }
    }
    virtual void bindApplication(String8 appName)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeString8(appName);
        remote()->transact(TRANSACTION_bindApplication, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // Something is wrong
        }
    }

    virtual void scheduleLaunchActivity(sp<ActivityInfo> ai, sp<IBinder> token, sp<Intent> intent)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        ai->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        _data.writeStrongBinder(token);
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        remote()->transact(TRANSACTION_scheduleLaunchActivity, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // Something is wrong
        }
    }

    virtual void scheduleCreateService(sp<ServiceInfo> si, sp<IBinder> token, sp<Intent> intent)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        si->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        _data.writeStrongBinder(token);
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        remote()->transact(TRANSACTION_scheduleCreateService, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // Something is wrong
        }
    }

    virtual void scheduleBindService(sp<IBinder> token, sp<Intent> intent, bool rebind)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(token);
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        if(rebind) {
            _data.writeInt32(1);
        } else {
            _data.writeInt32(0);
        }

        remote()->transact(TRANSACTION_scheduleBindService, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // Something is wrong
        }
    }
    virtual void scheduleReceiver(sp<Intent> intent, sp<Bundle> bundle, bool ordered, bool sticky)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        bundle->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        if(ordered) {
            _data.writeInt32(1);
        } else {
            _data.writeInt32(0);
        }

        if(sticky) {
            _data.writeInt32(1);
        } else {
            _data.writeInt32(0);
        }

        remote()->transact(TRANSACTION_scheduleBindService, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // Something is wrong
        }
    }

};


IMPLEMENT_META_INTERFACE(ApplicationThread, "com::cdroid::runtime::IApplicationThread");

int BnApplicationThread::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_schedulePauseActivity:
            {
                CHECK_INTERFACE(IApplicationThread, data, reply);
                sp<IBinder> _arg0 = data.readStrongBinder();
                schedulePauseActivity(_arg0);
                reply->writeInt32(1);
                return true;
            }
            break;
        case TRANSACTION_bindApplication:
            {
                CHECK_INTERFACE(IApplicationThread, data, reply);
                String8 _arg0 = data.readString8();
                bindApplication(_arg0);
                reply->writeInt32(1);
                return true;
            }
            break;
        case TRANSACTION_scheduleLaunchActivity:
            {
                CHECK_INTERFACE(IApplicationThread, data, reply);
                sp<ActivityInfo> _arg0;
                sp<IBinder> _arg1;
                sp<Intent> _arg2;
                _arg0 = _arg0->createFromParcel(data);
                _arg1 = data.readStrongBinder();
                _arg2 = _arg2->createFromParcel(data);
                scheduleLaunchActivity(_arg0, _arg1, _arg2);
                reply->writeInt32(1);
                return true;
            }
            break;
        case TRANSACTION_scheduleCreateService:
            {
                CHECK_INTERFACE(IApplicationThread, data, reply);
                sp<ServiceInfo> _arg0;
                sp<IBinder> _arg1;
                sp<Intent> _arg2;
                _arg0 = _arg0->createFromParcel(data);
                _arg1 = data.readStrongBinder();
                _arg2 = _arg2->createFromParcel(data);
                scheduleCreateService(_arg0, _arg1, _arg2);
                reply->writeInt32(1);
                return true;
            }
            break;
        case TRANSACTION_scheduleBindService:
            {
                CHECK_INTERFACE(IApplicationThread, data, reply);
                sp<IBinder> _arg0;
                sp<Intent> _arg1;
                bool _arg2;
                _arg0 = data.readStrongBinder();
                _arg1 = _arg1->createFromParcel(data);
                if(data.readInt32() == 1) {
                    _arg2 = true;
                } else {
                    _arg2 = false;
                }
                scheduleBindService(_arg0, _arg1, _arg2);
                reply->writeInt32(1);
                return true;
            }
            break;
        case TRANSACTION_scheduleReceiver:
            {
                CHECK_INTERFACE(IApplicationThread, data, reply);
                sp<Intent> _arg0;
                sp<Bundle> _arg1;
                bool _arg2;
                bool _arg3;
                _arg0 = _arg0->createFromParcel(data);
                _arg1 = _arg1->createFromParcel(data);
                if(data.readInt32() == 1) {
                    _arg2 = true;
                } else {
                    _arg2 = false;
                }
                if(data.readInt32() == 1) {
                    _arg3 = true;
                } else {
                    _arg3 = false;
                }
                scheduleReceiver(_arg0, _arg1, _arg2, _arg3);
                reply->writeInt32(1);
                return true;
            }
            break;
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
