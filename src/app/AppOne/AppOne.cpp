#define LOG_TAG "AppOne"
#include <cutils/log.h>

#include <runtime/AppCommon.h>


class AppOne : public Activity {
public:
    AppOne(sp<Intent> intent);

    virtual void onCreate(sp<Bundle> savedInstanceState);
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
};

AppOne::AppOne(sp<Intent> intent)
    : Activity(intent)
{
    ALOGI("constructor");
}

void AppOne::onCreate(sp<Bundle> savedInstanceState)
{

    Activity::onCreate(savedInstanceState);
}

void AppOne::onStart()
{
    Activity::onStart();
}

void AppOne::onResume()
{
    Activity::onResume();
}

void AppOne::onPause()
{
    Activity::onPause();
}

void AppOne::onStop()
{
    Activity::onStop();
}

void AppOne::onDestroy()
{
    Activity::onDestroy();
}

static Activity *createAppOneActivity(sp<Intent> intent)
{
    return new AppOne(intent);
}

static struct cdroid::ActivityManifest AppOneManifest= {
    String8("AppOneActivity"),
    String8("com.cdroid.app.appone"),
    Intent::CATEGORY_DEFAULT,
    NULL,
    createAppOneActivity
};

extern "C" {
cdroid::ApplicationManifest APPLICATION_INFO_SYM = {
    String8("AppOneApplication"),
    NULL,
    &AppOneManifest
};
};
