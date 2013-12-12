#define LOG_TAG "Service"
#include <cutils/log.h>

#include <runtime/Service.h>
#include <runtime/ActivityThread.h>


namespace cdroid {


Service::Service(sp<Intent> intent)
{
    mIntent = intent;
}


void Service::onCreate()
{
    ALOGI("onCreate");
}

void Service::onStart()
{
    ALOGI("onStart");
}

void Service::onStartCommand(sp<Intent> intent)
{
    ALOGI("onStartCommand");
}

sp<IBinder> Service::onBind(sp<Intent> intent)
{
    ALOGI("onBind");
}

void Service::onUnBind()
{
    ALOGI("onUnBind");
}

void Service::onDestroy()
{
    ALOGI("onDestroy");
}

int Service::attach(sp<Context> context, sp<ActivityThread> thread, sp<IBinder> token, sp<ServiceInfo> si)
{
    attachBaseContext(context);
    mThread = thread;
    mToken = token;
    mServiceInfo = si;
}


};


