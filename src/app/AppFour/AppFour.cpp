#define LOG_TAG "AppFour"
#include <cutils/log.h>

#include <runtime/AppCommon.h>

#include "ITestBindedService.h"

class AppFour : public Activity {
public:
    AppFour(sp<Intent> intent);

    virtual void onCreate(sp<Bundle> savedInstanceState);
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
};

class TestBindedService : public BnTestBind {
public:
    TestBindedService() {
        ALOGI("TestBindedService started");
    }
    virtual void sayHello()
    {
        ALOGI("hello from TestBindedService");
    }
};

class AppFourService : public Service
{
public:
    AppFourService(sp<Intent> intent);
    // life cycle callback
    virtual void onCreate();
    virtual void onStart();
    virtual void onStartCommand(sp<Intent> intent);
    virtual sp<IBinder> onBind(sp<Intent> intent);
    virtual void onUnBind();
    virtual void onDestroy();
private:
    sp<TestBindedService> mTService;
};


AppFour::AppFour(sp<Intent> intent)
    : Activity(intent)
{
    ALOGI("constructor");
}

void AppFour::onCreate(sp<Bundle> savedInstanceState)
{
    Activity::onCreate(savedInstanceState);
}

void AppFour::onStart()
{
    Activity::onStart();
}

void AppFour::onResume()
{
    Activity::onResume();
}

void AppFour::onPause()
{
    Activity::onPause();
}

void AppFour::onStop()
{
    Activity::onStop();
}

void AppFour::onDestroy()
{
    Activity::onDestroy();
}

AppFourService::AppFourService(sp<Intent> intent)
    : Service(intent)
{
    mTService = new TestBindedService();
}


void AppFourService::onCreate()
{
    Service::onCreate();
}

void AppFourService::onStart()
{
    Service::onStart();
}

void AppFourService::onStartCommand(sp<Intent> intent)
{
    Service::onStartCommand(intent);
}

sp<IBinder> AppFourService::onBind(sp<Intent> intent)
{
    ALOGI("AppFourService onbind");
    Service::onBind(intent);
    return mTService;
}

void AppFourService::onUnBind()
{
    Service::onUnBind();
}

void AppFourService::onDestroy()
{
    Service::onDestroy();
}

static Activity *createAppFourActivity(sp<Intent> intent)
{
    return new AppFour(intent);
}

static Service *createAppFourService(sp<Intent> intent)
{
    return new AppFourService(intent);
}


static struct cdroid::ActivityManifest AppFourManifest= {
    String8("AppFourActivity"),
    String8("com.cdroid.app.AppFour"),
    Intent::CATEGORY_DEFAULT,
    NULL,
    createAppFourActivity
};

struct cdroid::ServiceManifest AppFourServiceManifest = {
    String8("AppFourService"),
    String8("com.cdroid.app.service.appfour"),
    Intent::CATEGORY_DEFAULT,
    NULL,
    createAppFourService
};

extern "C" {
cdroid::ApplicationManifest APPLICATION_INFO_SYM = {
    String8("AppFourApplication"),
    NULL,
    &AppFourManifest,
    &AppFourServiceManifest
};
};
