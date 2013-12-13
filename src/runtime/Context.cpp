#define LOG_TAG "Context"
#include <cutils/log.h>

#include <runtime/Context.h>
#include <runtime/ActivityThread.h>

namespace cdroid {

int ContextWrapper::attachBaseContext(sp<Context> base)
{
    mBase = base;
}

int ContextWrapper::execInternalCommand(String8 cmd)
{
    assert(mBase != NULL);
    return mBase->execInternalCommand(cmd);
}

int ContextWrapper::startActivity(sp<Intent> intent)
{
    assert(mBase != NULL);
    return mBase->startActivity(intent);
}

int ContextWrapper::startService(sp<Intent> intent)
{
    assert(mBase != NULL);
    return mBase->startService(intent);
}

int ContextWrapper::bindService(sp<Intent> intent, sp<ServiceConnection> conn)
{
    assert(mBase != NULL);
    return mBase->bindService(intent, conn);
}


};
