#define LOG_TAG "PackageManager"
#include <cutils/log.h>

#include <binder/IServiceManager.h>
#include <service/PackageManager.h>


namespace cdroid {
sp<IPackageManager> PackageManager::sProxy;
Mutex PackageManager::mSingletonMutex;

PackageManager::PackageManager()
{
}

sp<IPackageManager> PackageManager::getDefault()
{
    if(!sProxy.get()) {
        AutoMutex _l(mSingletonMutex);
        if(!sProxy.get()) {
            sp<IServiceManager> sm = android::defaultServiceManager();
            sp<IBinder> const service = sm->checkService(String16("package"));

            sProxy = IPackageManager::asInterface(service);
            assert(sProxy.get() != NULL);
        }
    }
    return sProxy;
}

sp<ActivityInfo> PackageManager::getActivityInfo(String8 name)
{
    return getDefault()->getActivityInfo(name);
}

sp<ActivityInfo> PackageManager::resolveActivityInfo(String8 action)
{
    return getDefault()->resolveActivityInfo(action);
}

sp<ServiceInfo> PackageManager::resolveServiceInfo(String8 action)
{
    return getDefault()->resolveServiceInfo(action);
}


};
