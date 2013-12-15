#define LOG_TAG "IIntentReceiver"
#include <cutils/log.h>
#include <binder/IServiceManager.h>

#include <runtime/IIntentReceiver.h>

namespace cdroid {
enum {
    TRANSACTION_performReceive = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
};

class BpIntentReceiver: public BpInterface<IIntentReceiver>
{
public:
    BpIntentReceiver(const sp<IBinder>& impl)
        : BpInterface<IIntentReceiver>(impl)
    {
    }

    virtual void performReceive(sp<Intent> intent, sp<Bundle> extra, bool ordered, bool sticky)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        intent->writeToParcel(&_data, 0);
        extra->writeToParcel(&_data, 0);
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
        remote()->transact(TRANSACTION_performReceive, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // ERROR
        }
    }
};

IMPLEMENT_META_INTERFACE(IntentReceiver, "com::cdroid::service::internal::intentreceiver");

int BnIntentReceiver::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_performReceive:
            {
                CHECK_INTERFACE(IIntentReceiver, data, reply);
                sp<Intent> _arg0;
                sp<Bundle> _arg1;
                bool _arg2;
                bool _arg3;
                _arg0 = _arg0->createFromParcel(data);
                _arg1 = _arg1->createFromParcel(data);
                if(1 == data.readInt32()) {
                    _arg2 = true;
                } else {
                    _arg2 = true;
                }

                if(1 == data.readInt32()) {
                    _arg3 = true;
                } else {
                    _arg3 = true;
                }

                performReceive(_arg0, _arg1, _arg2, _arg3);
                reply->writeInt32(1);
                return true;
            }
            break;
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
