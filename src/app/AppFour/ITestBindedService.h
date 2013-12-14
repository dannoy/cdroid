#ifndef _APP_TEST_SERVICE_H
#define _APP_TEST_SERVICE_H

#include <runtime/AppCommon.h>

class ITestBind : public IInterface {
public:
    DECLARE_META_INTERFACE(TestBind);
    virtual void sayHello() = 0; 
};

class BnTestBind : public BnInterface<ITestBind> {
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};

#endif
