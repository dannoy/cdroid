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

sp<AppBindRecord> ServiceRecord::retrieveAppBindingLocked(sp<Intent> intent, sp<ProcessRecord> app)
{
    map<String8, sp<IntentBindRecord> >::iterator it = mBindings.find(intent->getAction());
    sp<IntentBindRecord> i;

    if(it != mBindings.end()) {
        i = it->second;
    }

    if(i == NULL) {
        i = new IntentBindRecord(this, intent);
        mBindings.insert(pair<intent->getAction(), i>);
    }

    map<sp<ProcessRecord>, sp<AppBindRecord> >::iterator it2 = i->apps.find(app);
    sp<AppBindRecord> a;

    if(it2 != i->apps.end()) {
        a = it2->second;
        return a;
    }

    a = new AppBindRecord(this, i, app);
    i->apps.insert(pair<sp<ProcessRecord>, sp<AppBindRecord> >(app, a));

    return a;
    
}

};
