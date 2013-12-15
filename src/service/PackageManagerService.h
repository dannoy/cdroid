#ifndef _SERVICE_PACKAGEMANAGER_SERVICE_H
#define _SERVICE_PACKAGEMANAGER_SERVICE_H

#include <service/IPackageManager.h>
#include <runtime/Activity.h>
#include <runtime/ApplicationLoader.h>


namespace cdroid {

class PackageManagerService : public BnPackageManager {
public:
    PackageManagerService();

    void addApplicationManifest(struct ApplicationManifest *appM, String8 filename);


    static void instantiate();

private:
    Vector<sp<ActivityInfo> > mActivities;
    Vector<ActivityManifest* > mActivityMF;
    Vector<sp<ServiceInfo> > mServices;
    Vector<ServiceManifest* > mServiceMF;
    Vector<sp<ReceiverInfo> > mReceivers;
    Vector<ReceiverManifest* > mReceiverMF;
    // TODO: use map to map item in mApplicationMF to 
    // item in mApplicationMF_file
    Vector<ApplicationManifest* > mApplicationMF;
    Vector<String8> mApplicationMF_file;

// IPackageManager Interfaces
public:
    virtual sp<ActivityInfo> getActivityInfo(String8 name); 
    virtual sp<ActivityInfo> resolveActivityInfo(String8 action); 
    virtual sp<ServiceInfo> resolveServiceInfo(String8 action);
    virtual Vector<sp<ReceiverInfo> >* resolveIntentReceivers(sp<Intent> intent);

};


};
#endif
