#ifndef _APP_TEST_SERVICE_H
#define _APP_TEST_SERVICE_H

#include <runtime/AppCommon.h>

class ITest : public IInterface {
public:
    DECLARE_META_INTERFACE(Test);
    virtual void sayHello() = 0; 
};

class BnTest : public BnInterface<ITest> {
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};

#endif
