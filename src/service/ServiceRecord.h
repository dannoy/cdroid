#ifndef _SERVICE_SERVICERECORD_H
#define _SERVICE_SERVICERECORD_H

#include <runtime/common.h>
#include <runtime/Intent.h>
#include <runtime/ServiceInfo.h>

#include "ProcessRecord.h"
#include "ActivityRecord.h"


namespace cdroid {
class ActivityManagerService;
class ActiveServices;
class ServiceRecord;
class IntentBindRecord;

class AppBindRecord : public RefBase {
public:
    AppBindRecord(sp<ServiceRecord> s, sp<IntentBindRecord> i, sp<ProcessRecord> p)
        : mServiceRecord(s),
        mIntentBindRecord(i),
        mApp(p)
    {
    }

    sp<ServiceRecord> mServiceRecord;
    sp<IntentBindRecord> mIntentBindRecord;
    sp<ProcessRecord> mApp;
};

class IntentBindRecord : public RefBase {
public:
    IntentBindRecord(sp<ServiceRecord> r, sp<Intent> intent)
        : mRecord(r),
        mIntent(intent),
        requested(false),
        received(false),
        hasBound(false)
    {
    }

    /*private:*/
    sp<ServiceRecord> mRecord;
    sp<Intent>       mIntent;

    map<sp<ProcessRecord>, sp<AppBindRecord> > apps;

    bool requested;
    bool received;
    bool hasBound;

    sp<IBinder> binder;
};

class ConnectionRecord : public RefBase {
public:
    ConnectionRecord( sp<AppBindRecord> appBindRecord, sp<ActivityRecord> activity, sp<IServiceConnection> c, int f)
        : mAppBindRecord(appBindRecord),
        mActivity(activity),
        mConnection(c),
        mFlags(f)
    {
    }
    sp<AppBindRecord> mAppBindRecord;
    sp<ActivityRecord> mActivity;
    sp<IServiceConnection> mConnection;
    int mFlags;
};

class ServiceRecord : public BBinder
{
public:
    ServiceRecord(sp<ActivityManagerService> service,
                    sp<ActiveServices> as,
                    sp<ServiceInfo> si, sp<Intent> intent,
                    sp<ProcessRecord> caller);

    sp<AppBindRecord> retrieveAppBindingLocked(sp<Intent> intent, sp<ProcessRecord> app);

    /*private:*/
    sp<ActivityManagerService> mService; // owner
    sp<ActiveServices> mActiveService; // owner
    sp<Intent> mIntent;
    sp<ProcessRecord> mCaller;
    sp<ProcessRecord> mApp;
    sp<ServiceInfo> mServiceInfo;

    map<String8, sp<IntentBindRecord> > mBindings;
    map<sp<IBinder>, Vector<sp<ConnectionRecord> >* > mConnections;
};



};

#endif
