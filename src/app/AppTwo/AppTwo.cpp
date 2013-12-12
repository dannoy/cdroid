#define LOG_TAG "AppTwo"
#include <cutils/log.h>

#include <runtime/AppCommon.h>


class AppTwo : public Activity {
public:
    AppTwo(sp<Intent> intent);

    virtual void onCreate(sp<Bundle> savedInstanceState);
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
};

AppTwo::AppTwo(sp<Intent> intent)
    : Activity(intent)
{
    ALOGI("constructor");
}

void AppTwo::onCreate(sp<Bundle> savedInstanceState)
{

    Activity::onCreate(savedInstanceState);
}

void AppTwo::onStart()
{
    Activity::onStart();
}

void AppTwo::onResume()
{
    Activity::onResume();
}

void AppTwo::onPause()
{
    Activity::onPause();
}

void AppTwo::onStop()
{
    Activity::onStop();
}

void AppTwo::onDestroy()
{
    Activity::onDestroy();
}

static Activity *createAppTwoActivity(sp<Intent> intent)
{
    return new AppTwo(intent);
}

static struct cdroid::ActivityManifest AppTwoManifest= {
    String8("AppTwoActivity"),
    String8("com.cdroid.app.apptwo"),
    Intent::CATEGORY_DEFAULT,
    NULL,
    createAppTwoActivity
};

extern "C" {
cdroid::ApplicationManifest APPLICATION_INFO_SYM = {
    String8("AppTwoApplication"),
    NULL,
    &AppTwoManifest
};
};
