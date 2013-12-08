#ifndef _SERVICE_ACTIVITY_RECORD_H
#define _SERVICE_ACTIVITY_RECORD_H

namespace cdroid {

class ActivityRecord : public RefBasee {
public:
    ActivityRecord(sp<ActivityManagerService> service, sp<ActivityStack> stack);
private:
    sp<ActivityManagerService> mService; // owner
    sp<ActivityStack> mStack; // owner
};


};
#endif
