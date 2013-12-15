#ifndef _RUNTIME_RECEIVER_INFO_H
#define _RUNTIME_RECEIVER_INFO_H

#include <runtime/common.h>
#include <runtime/IntentFilter.h>
#include <runtime/Intent.h>

namespace cdroid {

class ReceiverInfo : public Parcelable {
public:
    ReceiverInfo(String8 name, String8 filename);
    int describeContents();
    void writeToParcel(Parcel *out, int flags);
    ReceiverInfo *createFromParcel(const Parcel& source);
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
    String8 mFilename;
    sp<IntentFilter> mINF;
};



};

#endif
