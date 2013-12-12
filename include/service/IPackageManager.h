#ifndef _SERVICE_IPACKAGEMANAGER_H
#define _SERVICE_IPACKAGEMANAGER_H

#include <runtime/common.h>
#include <runtime/ActivityInfo.h>
#include <runtime/ServiceInfo.h>

namespace cdroid {


class IPackageManager : public IInterface {
public:
    DECLARE_META_INTERFACE(PackageManager);
    virtual sp<ActivityInfo> getActivityInfo(String8 name) = 0; 
    virtual sp<ActivityInfo> resolveActivityInfo(String8 action) = 0; 
    virtual sp<ServiceInfo> resolveServiceInfo(String8 action) = 0; 
};

class BnPackageManager : public BnInterface<IPackageManager> {
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};



};

#endif
