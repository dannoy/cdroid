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
    ALOGI("Found ApplicationManifest %s", appM->name.string());
    mService->addApplicationManifest(appM, filename);
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
    ALOGI("After looking for ");
}

void PackageManagerService::addApplicationManifest(struct ApplicationManifest *appM, String8 filename)
{
    mApplicationMF.push_back(appM);
    mApplicationMF_file.push_back(filename);

    struct ActivityManifest *amf = appM->activity;

    while(amf) {
        mActivityMF.push_back(amf);
        sp<ActivityInfo> ai = new ActivityInfo(amf->name, filename);
        ai->mAction = amf->action;
        ai->mCategory = amf->category;
        ai->mApplicationName = appM->name;

        mActivities.push_back(ai);

        amf = amf->next;
    }

    struct ServiceManifest *smf = appM->service;
    while(smf) {
        mServiceMF.push_back(smf);
        sp<ServiceInfo> si = new ServiceInfo(smf->name, filename);
        si->mAction = smf->action;
        si->mCategory = smf->category;
        si->mApplicationName = appM->name;

        mServices.push_back(si);

        smf = smf->next;
    }

    struct ReceiverManifest *rmf = appM->receiver;
    while(smf) {
        mReceiverMF.push_back(rmf);
        sp<ReceiverInfo> ri = new ReceiverInfo(rmf->name, filename);
        sp<IntentFilter> inf = new IntentFilter();
        struct IntentFilter_ManifestItem *it = rmf->inf;
        while(it) {
            inf->addAction(it->action);
            inf->addCategory(it->category);
        }

        ri->mApplicationName = appM->name;
        ri->mINF = inf;

        mReceivers.push_back(ri);

        rmf = rmf->next;
    }

}

void PackageManagerService::instantiate()
{
    android::defaultServiceManager()->addService(android::String16("package"), new PackageManagerService());
}
sp<ActivityInfo> PackageManagerService::getActivityInfo(String8 name)
{
    for(Vector<sp<ActivityInfo> >::iterator it = mActivities.begin(); it != mActivities.end(); ++it) {
        if((*it)->mName == name) {
            return *it;
        }
    }

    return NULL;
}

sp<ActivityInfo> PackageManagerService::resolveActivityInfo(String8 action)
{
    for(Vector<sp<ActivityInfo> >::iterator it = mActivities.begin(); it != mActivities.end(); ++it) {
        //ALOGI("Looking ActivityManifest action %s", (*it)->mAction.string());
        if((*it)->mAction == action) {
            return *it;
        }
    }

    return NULL;
}

sp<ServiceInfo> PackageManagerService::resolveServiceInfo(String8 action) 
{
    for(Vector<sp<ServiceInfo> >::iterator it = mServices.begin(); it != mServices.end(); ++it) {
        //ALOGI("Looking ActivityManifest action %s", (*it)->mAction.string());
        if((*it)->mAction == action) {
            return *it;
        }
    }

    return NULL;
}

Vector<sp<ReceiverInfo> >* PackageManagerService::resolveIntentReceivers(sp<Intent> intent)
{
    Vector<sp<ReceiverInfo> >* result = new Vector<sp<ReceiverInfo> >();

    for(Vector<sp<ReceiverInfo> >::iterator it = mReceivers.begin(); it != mReceivers.end(); ++it) {
        //ALOGI("Looking ActivityManifest action %s", (*it)->mAction.string());
        if((*it)->mINF->matchIntent(intent)) {
            result->push_back(*it);
        }
    }

    return result;
}

};
