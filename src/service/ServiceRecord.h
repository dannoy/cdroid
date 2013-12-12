#ifndef _SERVICE_SERVICERECORD_H
#define _SERVICE_SERVICERECORD_H

#include <runtime/common.h>
#include <runtime/Intent.h>
#include <runtime/ServiceInfo.h>

#include "ProcessRecord.h"


namespace cdroid {
class ActivityManagerService;
class ActiveServices;

class ServiceRecord : public BBinder
{
public:
    ServiceRecord(sp<ActivityManagerService> service,
                    sp<ActiveServices> as,
                    sp<ServiceInfo> si, sp<Intent> intent,
                    sp<ProcessRecord> caller);

    /*private:*/
    sp<ActivityManagerService> mService; // owner
    sp<ActiveServices> mActiveService; // owner
    sp<Intent> mIntent;
    sp<ProcessRecord> mCaller;
    sp<ProcessRecord> mApp;
    sp<ServiceInfo> mServiceInfo;
};



};

#endif
