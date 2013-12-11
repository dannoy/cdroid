#define LOG_TAG "Context"
#include <cutils/log.h>

#include <runtime/Context.h>
#include <runtime/ActivityThread.h>

namespace cdroid {

int ContextWrapper::attachBaseContext(sp<Context> base)
{
    mBase = base;
}

};
