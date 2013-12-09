#ifndef _SERVICE_ACTIVITY_STACK_H
#define _SERVICE_ACTIVITY_STACK_H

#include <runtime/common.h>
#include <runtime/Looper.h>

#include <runtime/Context.h>

#include "ActivityRecord.h"

namespace cdroid {

class ActivityManagerService;

class ActivityStack : public RefBase{

public:
    ActivityStack(sp<ActivityManagerService> service, sp<Context> context, bool mainStack, sp<Looper> looper);
    bool resumeTopActivityLocked(sp<ActivityRecord> prev);
private:
    bool topRunningActivityLocked(sp<ActivityRecord> notTop);

    sp<ActivityManagerService>  mService;
    sp<Context>                 mContext;
    bool                        mMainStack;
    sp<Handler>                 mHandler;
};


};



#endif
