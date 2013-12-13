#define LOG_TAG "ActiveServices"
#include <cutils/log.h>

#include <service/PackageManager.h>

#include "ActiveServices.h"
#include "ProcessRecord.h"
#include "ActivityManagerService.h"

namespace cdroid {
ActiveServices::ActiveServices(sp<ActivityManagerService> ams)
    : mAMS(ams)
{
    mPM = new PackageManager();
}

sp<ServiceRecord> ActiveServices::retrieveServiceLocked(sp<Intent> intent)
{
    for(Vector<sp<ServiceRecord> >::iterator it = mServices.begin(); it != mServices.end(); ++it) {
        if((*it)->mServiceInfo->mAction == intent->getAction()) {
            return *it;
        }
    }

    return NULL;
}

int ActiveServices::startServiceLocked(sp<IApplicationThread> caller, sp<Intent> intent, int pid, int uid)
{
    sp<ProcessRecord> callerApp;
    if(caller != NULL) {
        callerApp = mAMS->getRecordForAppLocked(caller);
    }

    sp<ServiceInfo> si = mPM->resolveServiceInfo(intent->getAction());
    if(si == NULL) {
        ALOGE("Cannot find any service with %s", intent->getAction().string());
        return -1;
    }
    ALOGI("Found service %s:%s define in %s with %s", si->mApplicationName.string(),
                                                    si->mName.string(),
                                                    si->mFilename.string(),
                                                    intent->getAction().string());



    sp<ServiceRecord> r = retrieveServiceLocked(intent);

    if(r != NULL)
    {
        if(r->mApp != NULL) {
            ALOGI("Found service %s:%s running in process %s", si->mApplicationName.string(),
                                                        si->mName.string(),
                                                        r->mApp->name.string());
        }
        else {
            ALOGI("Found service %s:%s is starting", si->mApplicationName.string(),
                                                        si->mName.string());
        }

        return 0;
    }

    r = new ServiceRecord(mAMS, this, si, intent, callerApp);

    return startServiceLocked(r);
}

int ActiveServices::startServiceLocked(sp<ServiceRecord> r)
{
    sp<ProcessRecord> app = mAMS->getProcessRecordLocked(r->mServiceInfo->mApplicationName);

    // Add before start process
    if(r->mApp == NULL) {
        mServices.push_back(r);
    }

    if(app == NULL) {
        mAMS->startProcessLocked(r->mServiceInfo->mApplicationName);
        return 0;
    } else if(r->mApp != NULL && r->mApp->thread != NULL) { // already started
        return 0;
    }



    return realStartServiceLocked(r, app);
}

int ActiveServices::attachApplicationLocked(sp<ProcessRecord> app)
{
    sp<ServiceRecord> r;

    if(app != NULL) {
        for(Vector<sp<ServiceRecord> >::iterator it = mServices.begin(); it != mServices.end(); ++it) {
            if((*it)->mServiceInfo->mApplicationName == app->name) {
                r = *it;
                break;
            }
        }
    }

    if(r == NULL) {
        ALOGE("No service request process pid %d", app->pid);
        return -1;
    }
    return realStartServiceLocked(r, app);
}

int ActiveServices::requestServiceBindingsLocked(sp<ServiceRecord> r)
{
    map<String8, sp<IntentBindRecord> >::iterator it = r->mBindings.begin();

    for(; it != r->mBindings.end(); ++it) {
        if(requestServiceBindingsLocked(r, *it, false) < 0) {
            break;
        }
    }
}

int ActiveServices::requestServiceBindingsLocked(sp<ServiceRecord> r, sp<IntentBindRecord> i, bool rebind)
{
    if(r->mApp == NULL || r->mApp->thread == NULL) {
        return -1;
    }

    if((!i->requested || rebind) && i->apps.size() > 0) {
       r->mApp->thread->scheduleBindService(r, i->mIntent, rebind);
       if(!rebind) {
            i->requested = true;
       }
       i->hasBound = true;
    }

}

int ActiveServices::realStartServiceLocked(sp<ServiceRecord> r, sp<ProcessRecord> app)
{

    r->mApp = app;
    //ALOGI("Activity %s request process pid %d started", r->mActivityInfo->mName.string(), app->pid);

    app->thread->scheduleCreateService(r->mServiceInfo, r, r->mIntent);

    requestServiceBindingsLocked(r);
    return 0;
}

int ActiveServices::bindServiceLocked(sp<IApplicationThread> caller, sp<IBinder> token, sp<Intent> intent, sp<IServiceConnection> connection, int pid, int uid, int flags)
{
    sp<ProcessRecord> callerApp;
    if(caller != NULL) {
        callerApp = mAMS->getRecordForAppLocked(caller);
    }

    sp<ServiceRecord> r = retrieveServiceLocked(intent);

    if(r == NULL) 
    {
        sp<ServiceInfo> si = mPM->resolveServiceInfo(intent->getAction());
        if(si == NULL) {
            ALOGE("Cannot find any service with %s", intent->getAction().string());
            return -1;
        }
        ALOGI("Found service %s:%s define in %s with %s", si->mApplicationName.string(),
                                                        si->mName.string(),
                                                        si->mFilename.string(),
                                                        intent->getAction().string());

        r = new ServiceRecord(mAMS, this, si, intent, callerApp);
    }

    sp<ActivityRecord> activity;
    if(token != NULL) {
        activity = mAMS->mMainStack->getActivityRecordByToken(token);
    }

    sp<AppBindRecord> b = r->retrieveAppBindingLocked(intent, callerApp);
    sp<ConnectionRecord> c = new ConnectionRecord(b, activity, conn, flags);

    sp<IBinder> binder = conn->asBinder();
    map<sp<IBinder>, Vector<sp<ConnectionRecord> >* >::iterator it = r->mConnections.find(b);
    Vector<sp<ConnectionRecord> > *clist = NULL;

    if(it == r->mConnections.end()) {
        clist = new Vector<ConnectionRecord>();
        r.mConnections.insert(pair<sp<IBinder>, Vector<ConnectionRecord>* >(binder, clist));
    }

    clist->push_back(c);

    startServiceLocked(r);

    if(r->mApp != NULL && b->mIntentBindRecord->received) {
        c->conn->connected(r->mName, b->mIntentBindRecord->binder);
    }
}

int ActiveServices::publishService(sp<IBinder> token, sp<Intent> intent, sp<IBinder> service)
{
    sp<ServiceRecord> r = reinterpret_cast<ServiceRecord*>(token.get());
    if(r == NULL) {
        ALOGE("ERROR: r == NULL");
        return -1;
    }

    map<String8, sp<IntentBindRecord> >::iterator it = r->mBindings.find(intent->getAction());
    if(it == r->mBindings.end()) {
        ALOGE("ERROR: cannot find record when processing publishService");
        return -1;
    }

    sp<IntentBindRecord> i = it->second;
    i->binder = service;
    i->received = true;

    map<sp<IBinder>, Vector<sp<ConnectionRecord> >* >::iterator it2 = mConnections.begin();

    for(; it2 != mConnections.end(); ++it2) {
        Vector<sp<ConnectionRecord> > *clist = it2->second;
        for(Vector<sp<ConnectionRecord> >::iterator it3 = clist->begin(); it3 != clist->end; ++it3) {
            sp<ConnectionRecord> c = *it3;
            c->mConnection->connected(r->mName, i->binder);
        }
    }

    return 0;
}

};
