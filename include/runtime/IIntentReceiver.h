#ifndef _RUNTIME_INTENTRECEIVER_H
#define _RUNTIME_INTENTRECEIVER_H

#include <runtime/Context.h>
#include <runtime/Bundle.h>

namespace cdroid {
class IIntentReceiver : public IInterface {
public:
    DECLARE_META_INTERFACE(IntentReceiver);
    virtual void performReceive(sp<Intent> intent, sp<Bundle> extra, bool ordered, bool sticky) = 0;
};

class BnIntentReceiver : public BnInterface<IIntentReceiver> {
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};

};

#endif
