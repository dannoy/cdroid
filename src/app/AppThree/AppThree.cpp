#define LOG_TAG "AppThree"
#include <cutils/log.h>

#include <runtime/AppCommon.h>

#include "ITestService.h"

class AppThree : public Activity {
public:
    AppThree(sp<Intent> intent);

    virtual void onCreate(sp<Bundle> savedInstanceState);
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
};

class AppThreeService : public Service
{
public:
    AppThreeService(sp<Intent> intent);
    // life cycle callback
    virtual void onCreate();
    virtual void onStart();
    virtual void onStartCommand(sp<Intent> intent);
    virtual sp<IBinder> onBind(sp<Intent> intent);
    virtual void onUnBind();
    virtual void onDestroy();
};

class TestService : public BnTest {
public:
    virtual void sayHello()
    {
        ALOGI("hello from TestService");
    }
    static void instantiate() {
        android::defaultServiceManager()->addService(android::String16("test"), new TestService());
    }
};

AppThree::AppThree(sp<Intent> intent)
    : Activity(intent)
{
    ALOGI("constructor");
}

void AppThree::onCreate(sp<Bundle> savedInstanceState)
{

    Activity::onCreate(savedInstanceState);
}

void AppThree::onStart()
{
    Activity::onStart();
}

void AppThree::onResume()
{
    Activity::onResume();
}

void AppThree::onPause()
{
    Activity::onPause();
}

void AppThree::onStop()
{
    Activity::onStop();
}

void AppThree::onDestroy()
{
    Activity::onDestroy();
}

AppThreeService::AppThreeService(sp<Intent> intent)
    : Service(intent)
{
}


void AppThreeService::onCreate()
{
    TestService::instantiate();
    Service::onCreate();
}

void AppThreeService::onStart()
{
    Service::onStart();
}

void AppThreeService::onStartCommand(sp<Intent> intent)
{
    Service::onStartCommand(intent);
}

sp<IBinder> AppThreeService::onBind(sp<Intent> intent)
{
    return Service::onBind(intent);
}

void AppThreeService::onUnBind()
{
    Service::onUnBind();
}

void AppThreeService::onDestroy()
{
    Service::onDestroy();
}

static Activity *createAppThreeActivity(sp<Intent> intent)
{
    return new AppThree(intent);
}

static Service *createAppThreeService(sp<Intent> intent)
{
    return new AppThreeService(intent);
}


static struct cdroid::ActivityManifest AppThreeManifest= {
    String8("AppThreeActivity"),
    String8("com.cdroid.app.appthree"),
    Intent::CATEGORY_DEFAULT,
    NULL,
    createAppThreeActivity
};

struct cdroid::ServiceManifest AppThreeServiceManifest = {
    String8("AppThreeService"),
    String8("com.cdroid.app.service.appthree"),
    Intent::CATEGORY_DEFAULT,
    NULL,
    createAppThreeService
};

extern "C" {
cdroid::ApplicationManifest APPLICATION_INFO_SYM = {
    String8("AppThreeApplication"),
    NULL,
    &AppThreeManifest,
    &AppThreeServiceManifest
};
};
