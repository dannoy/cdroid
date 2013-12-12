#ifndef _RUNTIME_SERVICE_H
#define _RUNTIME_SERVICE_H

#include <runtime/Context.h>
#include <runtime/Bundle.h>
#include <runtime/Intent.h>
#include <runtime/ServiceInfo.h>

namespace cdroid {
class ActivityThread;

class Service : public ContextWrapper
{
public:
    Service(sp<Intent> intent);
    // life cycle callback
    virtual void onCreate();
    virtual void onStart();
    virtual void onStartCommand(sp<Intent> intent);
    virtual sp<IBinder> onBind(sp<Intent> intent);
    virtual void onUnBind();
    virtual void onDestroy();

public:
    int attach(sp<Context> context, sp<ActivityThread> thread, sp<IBinder> token, sp<ServiceInfo> si);




private:
    sp<Intent> mIntent;
    sp<ActivityThread> mThread;
    sp<ServiceInfo> mServiceInfo;
    sp<IBinder> mToken;

};

};


#endif
