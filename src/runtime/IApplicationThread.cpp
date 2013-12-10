
#include <runtime/IApplicationThread.h>

namespace cdroid {
enum {
    TRANSACTION_schedulePauseActivity = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
    TRANSACTION_bindApplication,
    TRANSACTION_scheduleLaunchActivity,
};

class BpApplicationThread : public BpInterface<IApplicationThread> {
public:
    BpApplicationThread(const sp<IBinder>& impl)
        : BpInterface<IApplicationThread>(impl)
    {
    }
    void schedulePauseActivity(sp<IBinder> token)
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
    void bindApplication(String8 appName)
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

    void scheduleLaunchActivity(sp<ActivityInfo> ai)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        ai->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        remote()->transact(TRANSACTION_scheduleLaunchActivity, _data, &_reply, 0);
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
                _arg0 = _arg0->createFromParcel(data);
                scheduleLaunchActivity(_arg0);
                reply->writeInt32(1);
                return true;
            }
            break;
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
