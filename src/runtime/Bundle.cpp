#define LOG_TAG "Bundle"
#include <cutils/log.h>

#include <runtime/Bundle.h>

namespace cdroid {

Bundle::Bundle()
{
}

int Bundle::describeContents() {
    return 0;
}

void Bundle::writeToParcel(Parcel *out, int flags) {
}

Bundle *Bundle::createFromParcel(const Parcel& source){
    Bundle *i = new Bundle();
    return i;
}

void Bundle::readFromParcel(const Parcel& source) {
}

std::vector<Parcelable *> Bundle::newArray(int size) {
    std::vector<Parcelable *> v;
    for (int i = 0; i < size; ++i) {
        v.push_back(new Bundle());
    }

    return v;
}

};
