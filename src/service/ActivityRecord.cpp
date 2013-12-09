#define LOG_TAG "ActivityRecord"
#include <cutils/log.h>
#include "ActivityManagerService.h"

#include "ActivityRecord.h"

namespace cdroid {

ActivityRecord::ActivityRecord(sp<ActivityManagerService> service, sp<ActivityStack> stack)
{
    mService = service;
    mStack = stack;
}


};
