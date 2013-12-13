#ifndef _SERVICE_IACTIVITY_MANAGER_H
#define _SERVICE_IACTIVITY_MANAGER_H
#include <runtime/common.h>
#include <runtime/IApplicationThread.h>
#include <runtime/IServiceConnection.h>

namespace cdroid{

class IActivityManager : public IInterface{
public:
    DECLARE_META_INTERFACE(ActivityManager);
    virtual void attachApplication(sp<IBinder> appThread) = 0;
    virtual int startActivity(sp<IBinder> caller, sp<IBinder> resultTo, sp<Intent> intent, int requestCode) = 0;
    virtual int startService(sp<IBinder> caller, sp<Intent> intent) = 0;
    virtual int bindService(sp<IBinder> caller, sp<IBinder> token, sp<Intent> intent, sp<IBinder> connection, int flags) = 0;
};

class BnActivityManager : public BnInterface<IActivityManager> {
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};

class ActivityManagerNative : public BnActivityManager {
public:
    static sp<IActivityManager> getDefault();


    static sp<IActivityManager> sProxy;
    static Mutex mSingletonMutex;

};


};

#endif
