#define LOG_TAG "Launcher"
#include <cutils/log.h>

#include <runtime/Activity.h>
#include <runtime/ApplicationLoader.h>

using cdroid::Activity;
using cdroid::Bundle;

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
    (const char *)"LauncherActivity",
    NULL,
    createLauncherActivity
};

extern "C" {
cdroid::ApplicationManifest APPLICATION_INFO_SYM = {
    (const char *)"LauncherApplication",
    NULL,
    &launcherManifest
};
};
