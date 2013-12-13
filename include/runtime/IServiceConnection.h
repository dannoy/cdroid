#ifndef _RUNTIME_SERVICE_CONNECTION_H
#define _RUNTIME_SERVICE_CONNECTION_H

#include <runtime/Context.h>

namespace cdroid {
class IServiceConnection : public IInterface {
public:
    DECLARE_META_INTERFACE(ServiceConnection);
    virtual void connected(sp<ComponentName> name, sp<IBinder> service) = 0;
};

class BnServiceConnection : public BnInterface<IServiceConnection> {
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};

};

#endif
