#define LOG_TAG "ActivityManagerNative"
#include <cutils/log.h>

#include <service/IActivityManager.h>

namespace cdroid {

sp<IActivityManager> ActivityManagerNative::getDefault()
{
}

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


int BnActivityManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
