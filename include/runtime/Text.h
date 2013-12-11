#ifndef _RUNTIME_TEXT_H
#define _RUNTIME_TEXT_H

#include <runtime/common.h>

namespace cdroid {
class Text : public Parcelable {
public:
    Text();
    Text(String8& str);
    Text(const char* str);

    String8& getString();
    const char * getCharSequence();

    int describeContents();
    void writeToParcel(Parcel *out, int flags);
    Text *createFromParcel(const Parcel& source);
    void readFromParcel(const Parcel& source);
    std::vector<Parcelable *> newArray(int size);
private:
    String8 mString;

};

};

#endif
