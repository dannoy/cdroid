#define LOG_TAG "IDisplayManager"
#include <cutils/log.h>

#include <service/IDisplayManager.h>

namespace cdroid {
enum {
    TRANSACTION_displayText = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
};

class BpDisplayManager: public BpInterface<IDisplayManager>
{
public:
    BpDisplayManager(const sp<IBinder>& impl)
        : BpInterface<IDisplayManager>(impl)
    {
    }
    //virtual int displayText(sp<Text> txt)
    //{
        //Parcel _data;
        //Parcel _reply;
        //int _result = -1;

        //_data.writeInterfaceToken(this->getInterfaceDescriptor());
        //txt->writeToParcel(&_data, 0);
        //remote()->transact(TRANSACTION_displayText, _data, &_reply, 0);
        //_reply.readExceptionCode();
        //if ((0!=_reply.readInt32())) {
            //_result = _reply.readInt32();
        //}
        //else {
            //// ERROR
        //}
    //}
};

IMPLEMENT_META_INTERFACE(DisplayManager, "com::cdroid::service::IDisplayManager");

int BnDisplayManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    //switch(code) {
        //case TRANSACTION_displayText:
            //{
    //ALOGI("been request display");
                //CHECK_INTERFACE(IDisplayManager, data, reply);
                //sp<Text> txt;
                //txt = txt->createFromParcel(data);
    //ALOGI("been request display %s", txt->getCharSequence());
                //int result = displayText(txt);
                //reply->writeInt32(1);
                //reply->writeInt32(result);
                //return true;
            //}
            //break;

    //}
}


};
