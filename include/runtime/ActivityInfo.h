#ifndef _RUNTIME_ACTIVITY_INFO_H
#define _RUNTIME_ACTIVITY_INFO_H

#include <runtime/common.h>

namespace cdroid {

class ActivityInfo : public Parcelable {
public:
    ActivityInfo(String8 name, String8 filename);
    int describeContents();
    void writeToParcel(Parcel *out, int flags);
    ActivityInfo *createFromParcel(const Parcel& source);
    void readFromParcel(const Parcel& source);
    std::vector<Parcelable *> newArray(int size);

    String8& getName() {
        return mName;
    }

    String8& getFilename() {
        return mFilename;
    }

    String8 mName;
    String8 mApplicationName;
    String8 mAction;
    String8 mCategory;
    String8 mFilename;
};



};

#endif
