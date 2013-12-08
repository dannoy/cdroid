#ifndef _SERVICE_ACTIVITY_STACK_H
#define _SERVICE_ACTIVITY_STACK_H

namespace cdroid {

class ActivityManagerService;

class ActivityStack : public RefBase{

public:
    ActivityStack(sp<ActivityManagerService> service, sp<Context> context, bool mainStack, sp<Looper> looper);
    bool resumeTopActivityLocked(sp<ActivityRecord> prev);
private:
    bool topRunningActivityLocked(sp<ActivityRecord> notTop);
};


};



#endif
