#ifndef _SERVICE_IDISPLAYMANAGER_H
#define _SERVICE_IDISPLAYMANAGER_H

#include <runtime/common.h>
#include <runtime/Looper.h>
#include <runtime/Text.h>

namespace cdroid {
class IDisplayManager : public IInterface {
public:
    DECLARE_META_INTERFACE(DisplayManager);
    /*virtual int displayText(sp<Text> txt) = 0;*/

};

class BnDisplayManager : public BnInterface<IDisplayManager>
{
public:
    virtual int onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags);
};







};

#endif
