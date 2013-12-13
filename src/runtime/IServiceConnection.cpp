#define LOG_TAG "IServiceConnection"
#include <cutils/log.h>
#include <binder/IServiceManager.h>

#include <runtime/IServiceConnection.h>

namespace cdroid {
enum {
    TRANSACTION_connected = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
};

class BpServiceConnection: public BpInterface<IServiceConnection>
{
public:
    BpServiceConnection(const sp<IBinder>& impl)
        : BpInterface<IServiceConnection>(impl)
    {
    }

    virtual void connected(sp<ComponentName> name, sp<IBinder> service)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        name->writeToParcel(&_data, 0);
        _data.writeStrongBinder(service);
        remote()->transact(TRANSACTION_connected, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // ERROR
        }
    }
};

IMPLEMENT_META_INTERFACE(ServiceConnection, "com::cdroid::service::internal::serviceconnection");

int BnServiceConnection::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_connected:
            {
                CHECK_INTERFACE(IServiceConnection, data, reply);
                sp<ComponentName> _arg0;
                _arg0 = _arg0->createFromParcel(data);
                sp<IBinder> _arg1 = data.readStrongBinder();
                connected(_arg0, _arg1);
                reply->writeInt32(1);
                return true;
            }
            break;
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
