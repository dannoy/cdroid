#define LOG_TAG "PackageManagerService"
#include <cutils/log.h>
#include <binder/IServiceManager.h>

#include "PackageManagerService.h"
#include <runtime/ApplicationLoader.h>

namespace cdroid {

class PMS_ApplicationLoaderCallback : public ApplicationLoaderCallback {
public:
    PMS_ApplicationLoaderCallback(sp<PackageManagerService>& service);
    int onApplication(struct ApplicationManifest *appM, String8 filename);
private:
    sp<PackageManagerService> mService;
};
PMS_ApplicationLoaderCallback::PMS_ApplicationLoaderCallback(sp<PackageManagerService>& service)
{
    mService = service;
}

int PMS_ApplicationLoaderCallback::onApplication(struct ApplicationManifest *appM, String8 filename)
{
    ALOGI("Found ApplicationManifest %s", appM->name);
}

PackageManagerService::PackageManagerService()
{
    sp<PackageManagerService> self(this);
    /*
     * Notice HERE:
     * we should call incStrong to increment ref by one,
     * or ref will be 0 after this constructor.
     */
    self->incStrong(this);
    sp<ApplicationLoader> appLoader = new ApplicationLoader;
    sp<PMS_ApplicationLoaderCallback> cb = new PMS_ApplicationLoaderCallback(self);
    appLoader->loadApplications(cb);
}

void PackageManagerService::instantiate()
{
    android::defaultServiceManager()->addService(android::String16("package"), new PackageManagerService());
    //ALOGI("PMS added");
}
sp<ActivityInfo> PackageManagerService::getActivityInfo(String8 name)
{
    sp<ActivityInfo> ai = new ActivityInfo;

    return ai;
}


};
