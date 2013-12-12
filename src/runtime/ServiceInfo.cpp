#define LOG_TAG "ServiceInfo"
#include <cutils/log.h>

#include <runtime/ServiceInfo.h>

namespace cdroid {
ServiceInfo::ServiceInfo(String8 name, String8 filename)
    : mName(name), mFilename(filename)
{
}
void ServiceInfo::writeToParcel(Parcel *out, int flags) {
    out->writeString8(mName);
    out->writeString8(mApplicationName);
    out->writeString8(mFilename);
}

int ServiceInfo::describeContents() {
    return 0;
}

ServiceInfo *ServiceInfo::createFromParcel(const Parcel& source){
    String8 name = source.readString8();
    String8 appname = source.readString8();
    String8 f = source.readString8();
    ServiceInfo *ai = new ServiceInfo(name,f);
    ai->mApplicationName = appname;

    return ai;
}

void ServiceInfo::readFromParcel(const Parcel& source) {
    this->mName =  source.readString8();
    this->mApplicationName =  source.readString8();
    this->mFilename =  source.readString8();
}

std::vector<Parcelable *> ServiceInfo::newArray(int size) {
    std::vector<Parcelable *> v;
    for (int i = 0; i < size; ++i) {
        v.push_back(new ServiceInfo(String8(""),String8("")));
    }

    return v;
}

};
