#define LOG_TAG "ITest"
#include <cutils/log.h>

#include "ITestService.h"

enum {
    TRANSACTION_sayHello = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
};

class BpTest: public BpInterface<ITest>
{
public:
    BpTest(const sp<IBinder>& impl)
        : BpInterface<ITest>(impl)
    {
    }
    virtual void sayHello()
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        remote()->transact(TRANSACTION_sayHello, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            //error
        }
    }
};

IMPLEMENT_META_INTERFACE(Test, "com::cdroid::app::service::ITest");

int BnTest::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_sayHello:
            {
                CHECK_INTERFACE(IPackageManager, data, reply);
                sayHello();
                reply->writeInt32(1);
                return true;
            }
            break;
    }
    return BBinder::onTransact(code, data, reply, flags);

}
