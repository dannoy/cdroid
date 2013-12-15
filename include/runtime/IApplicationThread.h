#ifndef _RUNTIME_APPLICATION_THREAD_H
#define _RUNTIME_APPLICATION_THREAD_H

#include <runtime/common.h>
#include <runtime/ActivityInfo.h>
#include <runtime/ServiceInfo.h>
#include <runtime/Intent.h>
#include <runtime/Bundle.h>

namespace cdroid{

class IApplicationThread : public IInterface{
public:
    DECLARE_META_INTERFACE(ApplicationThread);
    virtual void schedulePauseActivity(sp<IBinder> token) = 0;
    virtual void bindApplication(String8 appName) = 0;
    virtual void scheduleLaunchActivity(sp<ActivityInfo> ai, sp<IBinder> token, sp<Intent> intent) = 0;
    virtual void scheduleCreateService(sp<ServiceInfo> ai, sp<IBinder> token, sp<Intent> intent) = 0;
    virtual void scheduleBindService(sp<IBinder> token, sp<Intent> intent, bool rebind) = 0;
    virtual void scheduleReceiver(sp<Intent> intent, sp<Bundle> bundle, bool ordered, bool sticky) = 0;
};

class BnApplicationThread : public BnInterface<IApplicationThread> {
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};

};

#endif
