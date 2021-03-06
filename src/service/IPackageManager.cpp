#define LOG_TAG "IPackageManager"
#include <cutils/log.h>

#include <service/IPackageManager.h>

namespace cdroid {
enum {
    TRANSACTION_getActivityInfo = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
    TRANSACTION_resolveActivityInfo,
    TRANSACTION_resolveServiceInfo,
    TRANSACTION_resolveIntentReceivers,
};

class BpPackageManager: public BpInterface<IPackageManager>
{
public:
    BpPackageManager(const sp<IBinder>& impl)
        : BpInterface<IPackageManager>(impl)
    {
    }
    virtual sp<ActivityInfo> getActivityInfo(String8 name)
    {
        Parcel _data;
        Parcel _reply;
        sp<ActivityInfo> _result;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeString8(name);
        remote()->transact(TRANSACTION_getActivityInfo, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _result->createFromParcel(_reply);
        }
        else {
            _result = NULL;
        }

        return _result;
    }

    virtual sp<ActivityInfo> resolveActivityInfo(String8 action)
    {
        Parcel _data;
        Parcel _reply;
        sp<ActivityInfo> _result;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeString8(action);
        remote()->transact(TRANSACTION_resolveActivityInfo, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _result->createFromParcel(_reply);
        }
        else {
            _result = NULL;
        }

        return _result;
    }

    virtual sp<ServiceInfo> resolveServiceInfo(String8 action)
    {
        Parcel _data;
        Parcel _reply;
        sp<ServiceInfo> _result;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeString8(action);
        remote()->transact(TRANSACTION_resolveServiceInfo, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _result->createFromParcel(_reply);
        }
        else {
            _result = NULL;
        }

        return _result;
    }

    virtual Vector<sp<ReceiverInfo> >* resolveIntentReceivers(sp<Intent> intent)
    {
        Parcel _data;
        Parcel _reply;
        Vector<sp<ReceiverInfo> >* _result = new Vector<sp<ReceiverInfo> >();

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        intent->writeToParcel(&_data, 0);
        remote()->transact(TRANSACTION_resolveIntentReceivers, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            int N = _reply.readInt32();
            while(N--) {
                sp<ReceiverInfo> rf;
                rf = rf->createFromParcel(_reply);
                _result->push_back(rf);
            }
        }
        else {
            _result = NULL;
        }

        return _result;
    }
};

IMPLEMENT_META_INTERFACE(PackageManager, "com::cdroid::service::IPackageManager");

int BnPackageManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_getActivityInfo:
            {
                CHECK_INTERFACE(IPackageManager, data, reply);
                String8 name = data.readString8();
                sp<ActivityInfo> _result = getActivityInfo(name);
                if(_result != NULL) {
                    reply->writeInt32(1);
                    _result->writeToParcel(reply, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
                } else {
                    reply->writeInt32(0);
                }
                return true;
            }
            break;
        case TRANSACTION_resolveActivityInfo:
            {
                CHECK_INTERFACE(IPackageManager, data, reply);
                String8 name = data.readString8();
                sp<ActivityInfo> _result = resolveActivityInfo(name);
                if(_result != NULL) {
                    reply->writeInt32(1);
                    _result->writeToParcel(reply, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
                } else {
                    reply->writeInt32(0);
                }
                return true;
            }
            break;
        case TRANSACTION_resolveServiceInfo:
            {
                CHECK_INTERFACE(IPackageManager, data, reply);
                String8 name = data.readString8();
                sp<ServiceInfo> _result = resolveServiceInfo(name);
                if(_result != NULL) {
                    reply->writeInt32(1);
                    _result->writeToParcel(reply, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
                } else {
                    reply->writeInt32(0);
                }
                return true;
            }
            break;
        case TRANSACTION_resolveIntentReceivers:
            {
                CHECK_INTERFACE(IPackageManager, data, reply);
                sp<Intent> _arg0;
                _arg0 = _arg0->createFromParcel(data);
                Vector<sp<ReceiverInfo> >* _result = resolveIntentReceivers(_arg0);
                if(_result != NULL) {
                    reply->writeInt32(1);
                    int N = _result->size();
                    reply->writeInt32(N);
                    while(N--) {
                        (*_result)[N]->writeToParcel(reply, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
                    }
                } else {
                    reply->writeInt32(0);
                }
                return true;
            }
            break;
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
