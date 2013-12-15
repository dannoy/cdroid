#ifndef _RUNTIME_BUNDLE_H
#define _RUNTIME_BUNDLE_H

#include <runtime/common.h>

namespace cdroid {

class Bundle : public Parcelable {
public:
    Bundle();
    int describeContents();
    void writeToParcel(Parcel *out, int flags);
    Bundle *createFromParcel(const Parcel& source);
    void readFromParcel(const Parcel& source);
    std::vector<Parcelable *> newArray(int size);
};


};

#endif
