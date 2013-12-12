#ifndef _SERVICE_ACTIVESERVICES_H
#define _SERVICE_ACTIVESERVICES_H

#include <runtime/Intent.h>
#include "ServiceRecord.h"
#include "ProcessRecord.h"

namespace cdroid {

class ActivityManagerService;

class ActiveServices : public RefBase
{
public:
    ActiveServices(sp<ActivityManagerService> ams);

    int attachApplicationLocked(sp<ProcessRecord> app);
    int startServiceLocked(sp<IApplicationThread> caller, sp<Intent> intent, int pid, int uid);

protected:

    sp<ServiceRecord> retrieveServiceLocked(sp<Intent> intent);

private:
    int startServiceLocked(sp<ServiceRecord> r);
    int realStartServiceLocked(sp<ServiceRecord> r, sp<ProcessRecord> app);

private:
    sp<ActivityManagerService> mAMS;

    Vector<sp<ServiceRecord> > mServices;
    sp<PackageManager>          mPM;
};



};

#endif
