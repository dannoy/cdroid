#define LOG_TAG "ReceiverInfo"
#include <cutils/log.h>

#include <runtime/ReceiverInfo.h>

namespace cdroid {
ReceiverInfo::ReceiverInfo(String8 name, String8 filename)
    : mName(name), mFilename(filename)
{
}
void ReceiverInfo::writeToParcel(Parcel *out, int flags) {
    out->writeString8(mName);
    out->writeString8(mApplicationName);
    out->writeString8(mFilename);
    mINF->writeToParcel(out, 0);
}

int ReceiverInfo::describeContents() {
    return 0;
}

ReceiverInfo *ReceiverInfo::createFromParcel(const Parcel& source){
    String8 name = source.readString8();
    String8 appname = source.readString8();
    String8 f = source.readString8();
    sp<IntentFilter> inf;
    inf = inf->createFromParcel(source);
    ReceiverInfo *ri = new ReceiverInfo(name,f);
    ri->mApplicationName = appname;
    ri->mINF = inf;

    return ri;
}

void ReceiverInfo::readFromParcel(const Parcel& source) {
    this->mName =  source.readString8();
    this->mApplicationName =  source.readString8();
    this->mFilename =  source.readString8();
    this->mINF = this->mINF->createFromParcel(source);
}

std::vector<Parcelable *> ReceiverInfo::newArray(int size) {
    std::vector<Parcelable *> v;
    for (int i = 0; i < size; ++i) {
        v.push_back(new ReceiverInfo(String8(""),String8("")));
    }

    return v;
}

};
