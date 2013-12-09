#define LOG_TAG "IPackageManager"
#include <cutils/log.h>

#include <service/IPackageManager.h>

namespace cdroid {

class BpPackageManager: public BpInterface<IPackageManager>
{
public:
    BpPackageManager(const sp<IBinder>& impl)
        : BpInterface<IPackageManager>(impl)
    {
    }
    virtual sp<ActivityInfo> getActivityInfo(String8 name)
    {
    }
};

IMPLEMENT_META_INTERFACE(PackageManager, "com::cdroid::service::IPackageManager");

int BnPackageManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
