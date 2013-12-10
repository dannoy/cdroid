#ifndef _RUNTIME_APPLICATION_THREAD_H
#define _RUNTIME_APPLICATION_THREAD_H

#include <runtime/common.h>

namespace cdroid{

class IApplicationThread : public IInterface{
public:
    DECLARE_META_INTERFACE(ApplicationThread);
    virtual void schedulePauseActivity(sp<IBinder> token) = 0;
};

class BnApplicationThread : public BnInterface<IApplicationThread> {
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};

};

#endif
