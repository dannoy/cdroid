#define LOG_TAG "Launcher"
#include <cutils/log.h>

#include <runtime/AppCommon.h>


class Launcher : public Activity {
    virtual void onCreate(Bundle savedInstanceState);
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();
};

void Launcher::onCreate(Bundle savedInstanceState)
{
}

void Launcher::onStart()
{
}

void Launcher::onResume()
{
}

void Launcher::onPause()
{
}

void Launcher::onStop()
{
}

void Launcher::onDestroy()
{
}

static Activity *createLauncherActivity()
{
    return new Launcher;
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
