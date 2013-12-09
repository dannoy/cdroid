#ifndef _SERVICE_PACKAGEMANAGER_SERVICE_H
#define _SERVICE_PACKAGEMANAGER_SERVICE_H

#include <service/IPackageManager.h>
#include <runtime/Activity.h>


namespace cdroid {

class PackageManagerService : public BnPackageManager {
public:
    PackageManagerService();
    static void instantiate();

private:
    Vector<sp<Activity> > mActivities;

// IPackageManager Interfaces
public:
    virtual sp<ActivityInfo> getActivityInfo(String8 name); 

};


};
#endif
