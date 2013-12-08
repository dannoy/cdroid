#define LOG_TAG "ActivityStack"
#include <cutils/log.h>

#include "ActivityStack.h"

namespace cdroid {

ActivityStack::ActivityStack(sp<ActivityManagerService> service, sp<Context> context, bool mainStack, sp<Looper> looper)
{
    mService = service;
    mContext = context;
    mMainStack = mainStack;
    mHandle = new ActivityStackHandler(looper);
}


};
