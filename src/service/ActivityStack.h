#ifndef _SERVICE_ACTIVITY_STACK_H
#define _SERVICE_ACTIVITY_STACK_H

#include <runtime/common.h>
#include <runtime/Looper.h>

#include <runtime/Context.h>
#include <runtime/Intent.h>
#include <runtime/IApplicationThread.h>

#include <service/PackageManager.h>

#include "ActivityRecord.h"

namespace cdroid {

class ActivityManagerService;

class ActivityStack : public RefBase{

public:
    ActivityStack(sp<ActivityManagerService> service, sp<Context> context, bool mainStack, sp<Looper> looper);
    bool resumeTopActivityLocked(sp<ActivityRecord> prev);
    int startActivityLocked(sp<IApplicationThread> caller, sp<Intent> intent, sp<IBinder> resultTo, int callingPid, int callingUid, int request);
    int startActivityLocked(sp<ActivityRecord> r, sp<ActivityRecord> sourceRecord);
    int realStartActivityLocked(sp<ActivityRecord> r, sp<ProcessRecord> app);
private:
    sp<ActivityRecord> topRunningActivityLocked(sp<ActivityRecord> notTop);
    int indexOfTokenLocked(sp<IBinder> token);

    sp<ActivityManagerService>  mService;
    sp<Context>                 mContext;
    bool                        mMainStack;
    sp<Handler>                 mHandler;
    Vector<sp<ActivityRecord> > mHistory;
    sp<PackageManager>          mPM;
};


};



#endif
