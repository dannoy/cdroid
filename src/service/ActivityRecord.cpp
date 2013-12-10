#define LOG_TAG "ActivityRecord"
#include <cutils/log.h>
#include "ActivityManagerService.h"

#include "ActivityRecord.h"

namespace cdroid {

class BpApplicationToken : public BpInterface<IApplicationToken>
{
public:
    BpApplicationToken(const sp<IBinder>& impl)
        : BpInterface<IApplicationToken>(impl)
    {
    }
};

IMPLEMENT_META_INTERFACE(ApplicationToken, "com::cdroid::service::IApplicationToken");

ActivityRecord::Token::Token(sp<ActivityRecord> r)
{
    mActivity = r;
}

int ActivityRecord::BnApplicationToken::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    //CHECK_INTERFACE(IApplicationToken, data, reply);
    switch(code) {
    }

    return BBinder::onTransact(code, data, reply, flags);
}

ActivityRecord:: ActivityRecord(sp<ActivityManagerService> service, sp<ActivityStack> stack, 
                    sp<ActivityInfo> ai, sp<Intent> intent,
                    sp<ProcessRecord> caller, sp<ActivityRecord> resultTo)
{
    mService = service;
    mStack = stack;
    mToken = new Token(this);

    mIntent = intent;
    mCaller = caller;
    mResultTo = resultTo;
    mActivityInfo = ai;
}


};
