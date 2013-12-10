#ifndef _SERVICE_ACTIVITY_RECORD_H
#define _SERVICE_ACTIVITY_RECORD_H

#include <runtime/common.h>

#include "ProcessRecord.h"

namespace cdroid {

class ActivityManagerService;
class ActivityStack;

class IApplicationToken : public IInterface {
public:
    DECLARE_META_INTERFACE(ApplicationToken);
};


class ActivityRecord : public RefBase {
public:

    class BnApplicationToken : public BnInterface<IApplicationToken> {
    public:
        virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
    };

    class Token : public BnApplicationToken {
    public:
        Token(sp<ActivityRecord> r);
        sp<ActivityRecord> mActivity;
    };
public:
    ActivityRecord(sp<ActivityManagerService> service, sp<ActivityStack> stack,
                    sp<ActivityInfo> ai, sp<Intent> intent,
                    sp<ProcessRecord> caller, sp<ActivityRecord> resultTo);
    /*private:*/
    sp<ActivityManagerService> mService; // owner
    sp<ActivityStack> mStack; // owner
    sp<BnApplicationToken> mToken;
    sp<Intent> mIntent;
    sp<ProcessRecord> mCaller;
    sp<ActivityRecord> mResultTo;
    sp<ProcessRecord> mApp;
    sp<ActivityInfo> mActivityInfo;
};


};
#endif
