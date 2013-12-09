#define LOG_TAG "ActivityStack"
#include <cutils/log.h>

#include "ActivityStack.h"
#include "ActivityManagerService.h"

namespace cdroid {

class ActivityStackHandler : public Handler {
public:
    ActivityStackHandler(sp<Looper> looper)
        : Handler(looper)
    {
    }
    virtual void handleMessage(const Message& message) {
        switch(message.what) {
        }
    }
};

ActivityStack::ActivityStack(sp<ActivityManagerService> service, sp<Context> context, bool mainStack, sp<Looper> looper)
{
    mService = service;
    mContext = context;
    mMainStack = mainStack;
    mHandler = new ActivityStackHandler(looper);
}

bool ActivityStack::resumeTopActivityLocked(sp<ActivityRecord> prev)
{
    return true;
}

bool ActivityStack::topRunningActivityLocked(sp<ActivityRecord> notTop)
{
    return true;
}


};
