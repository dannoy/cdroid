#define LOG_TAG "ActivityManagerService"
#include <cutils/log.h>

#include <service/IActivityManager.h>
#include <runtime/Process.h>
#include "ActivityManagerService.h"

namespace cdroid {

ActivityManagerService::ActivityManagerService()
{
    MY_PID = Process::myPid();
}

int ActivityManagerService::attachApplication(sp<IApplicationThread> appThread)
{
}

};
