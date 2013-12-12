#define LOG_TAG "ServiceRecord"
#include <cutils/log.h>
#include "ActivityManagerService.h"

#include "ServiceRecord.h"

namespace cdroid {
ServiceRecord::ServiceRecord(sp<ActivityManagerService> service, sp<ActiveServices> as, sp<ServiceInfo> si, sp<Intent> intent, sp<ProcessRecord> caller)
    : mService(service),
      mActiveService(as),
      mServiceInfo(si),
      mIntent(intent),
      mCaller(caller)
{
}

};
