#define LOG_TAG "ActivityManagerNative"
#include <cutils/log.h>
#include <binder/IServiceManager.h>

#include <service/IActivityManager.h>

namespace cdroid {
enum {
    TRANSACTION_attachApplication = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
};

class BpActivityManager: public BpInterface<IActivityManager>
{
public:
    BpActivityManager(const sp<IBinder>& impl)
        : BpInterface<IActivityManager>(impl)
    {
    }
    virtual void attachApplication(sp<IBinder> appThread)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(appThread);
        remote()->transact(TRANSACTION_attachApplication, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // ERROR
        }
    }
};

IMPLEMENT_META_INTERFACE(ActivityManager, "com::cdroid::service::IActivityManager");
sp<IActivityManager> ActivityManagerNative::sProxy;
Mutex ActivityManagerNative::mSingletonMutex;

sp<IActivityManager> ActivityManagerNative::getDefault()
{
    if(!sProxy.get()) {
        AutoMutex _l(mSingletonMutex);
        if(!sProxy.get()) {
            sp<IServiceManager> sm = android::defaultServiceManager();
            sp<IBinder> const service = sm->checkService(String16("activity"));
            sProxy = IActivityManager::asInterface(service);
            assert(sProxy.get() != NULL);
        }
    }
    return sProxy;
}


int BnActivityManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_attachApplication:
            {
                CHECK_INTERFACE(IActivityManager, data, reply);
                sp<IBinder> _arg0 = data.readStrongBinder();
                attachApplication(_arg0);
                reply->writeInt32(1);
                return true;
            }
            break;
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
