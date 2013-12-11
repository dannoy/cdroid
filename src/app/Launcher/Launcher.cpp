#define LOG_TAG "Launcher"
#include <cutils/log.h>

#include <runtime/AppCommon.h>


class Launcher : public Activity {
public:
    Launcher(sp<Intent> intent);

    virtual void onCreate(sp<Bundle> savedInstanceState);
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
};

Launcher::Launcher(sp<Intent> intent)
    : Activity(intent)
{
    ALOGI("constructor");
}

void Launcher::onCreate(sp<Bundle> savedInstanceState)
{

    Activity::onCreate(savedInstanceState);
}

void Launcher::onStart()
{
    Activity::onStart();
}

void Launcher::onResume()
{
    Activity::onResume();
}

void Launcher::onPause()
{
    Activity::onPause();
}

void Launcher::onStop()
{
    Activity::onStop();
}

void Launcher::onDestroy()
{
    Activity::onDestroy();
}

static Activity *createLauncherActivity(sp<Intent> intent)
{
    return new Launcher(intent);
}

static struct cdroid::ActivityManifest launcherManifest= {
    String8("LauncherActivity"),
    Intent::ACTION_MAIN,
    Intent::CATEGORY_DEFAULT,
    NULL,
    createLauncherActivity
};

extern "C" {
cdroid::ApplicationManifest APPLICATION_INFO_SYM = {
    String8("LauncherApplication"),
    NULL,
    &launcherManifest
};
};
