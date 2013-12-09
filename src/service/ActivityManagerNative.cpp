#define LOG_TAG "ActivityManagerNative"
#include <cutils/log.h>
#include <binder/IServiceManager.h>

#include <service/IActivityManager.h>

namespace cdroid {

class BpActivityManager: public BpInterface<IActivityManager>
{
public:
    BpActivityManager(const sp<IBinder>& impl)
        : BpInterface<IActivityManager>(impl)
    {
    }
    virtual int attachApplication(sp<IApplicationThread> appThread)
    {
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

            sProxy = new BpActivityManager(service);
            assert(sProxy.get() != NULL);
        }
    }
    return sProxy;
}


int BnActivityManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
