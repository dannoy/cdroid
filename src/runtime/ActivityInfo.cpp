#define LOG_TAG "ActivityInfo"
#include <cutils/log.h>

#include <runtime/ActivityInfo.h>

namespace cdroid {
ActivityInfo::ActivityInfo(String8 name, String8 filename)
    : mName(name), mFilename(filename)
{
}
void ActivityInfo::writeToParcel(Parcel *out, int flags) {
    out->writeString8(mName);
    out->writeString8(mApplicationName);
    out->writeString8(mFilename);
}

int ActivityInfo::describeContents() {
    return 0;
}

ActivityInfo *ActivityInfo::createFromParcel(Parcel& source){
    String8 name = source.readString8();
    String8 appname = source.readString8();
    String8 f = source.readString8();
    ActivityInfo *ai = new ActivityInfo(name,f);
    ai->mApplicationName = appname;

    return ai;
}

void ActivityInfo::readFromParcel(Parcel& source) {
    this->mName =  source.readString8();
    this->mApplicationName =  source.readString8();
    this->mFilename =  source.readString8();
}

std::vector<Parcelable *> ActivityInfo::newArray(int size) {
    std::vector<Parcelable *> v;
    for (int i = 0; i < size; ++i) {
        v.push_back(new ActivityInfo(String8(""),String8("")));
    }

    return v;
}

};
