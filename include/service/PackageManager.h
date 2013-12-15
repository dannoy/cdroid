#ifndef _SERVICE_PACKAGEMANAGER_H
#define _SERVICE_PACKAGEMANAGER_H

#include <service/IPackageManager.h>

#include <runtime/ReceiverInfo.h>


namespace cdroid {
class PackageManager : public RefBase{
public:
    PackageManager();
    sp<ActivityInfo> getActivityInfo(String8 name); 
    sp<ActivityInfo> resolveActivityInfo(String8 action); 
    sp<ServiceInfo> resolveServiceInfo(String8 action);
    Vector<sp<ReceiverInfo> >* resolveIntentReceivers(sp<Intent> intent);
private:
    static sp<IPackageManager> getDefault();
    static sp<IPackageManager> sProxy;
    static Mutex mSingletonMutex;

};



};

#endif
