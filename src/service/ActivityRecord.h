#ifndef _SERVICE_ACTIVITY_RECORD_H
#define _SERVICE_ACTIVITY_RECORD_H

#include <runtime/common.h>

namespace cdroid {

class ActivityManagerService;
class ActivityStack;

class ActivityRecord : public RefBase {
public:
    ActivityRecord(sp<ActivityManagerService> service, sp<ActivityStack> stack);
private:
    sp<ActivityManagerService> mService; // owner
    sp<ActivityStack> mStack; // owner
};


};
#endif
