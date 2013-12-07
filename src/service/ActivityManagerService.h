#ifndef _SERVICE_ACTIVITY_MANAGER_SERVICE_H
#define _SERVICE_ACTIVITY_MANAGER_SERVICE_H

#include <service/IActivityManager.h>

namespace cdroid {

class ActivityManagerService : public ActivityManagerNative {
    /*class ActivityManagerService : public BnInterface<IActivityManager> {*/
public:
    ActivityManagerService();
private:

    pid_t MY_PID;
};



};

#endif
