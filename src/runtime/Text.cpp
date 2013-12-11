#define LOG_TAG "Text"
#include <cutils/log.h>

#include <runtime/Text.h>


namespace cdroid {

Text::Text()
    : mString("")
{
}

Text::Text(String8& str)
    : mString(str)
{
}

Text::Text(const char* str)
    : mString(str)
{
}

String8& Text::getString()
{
    return mString;
}

const char* Text::getCharSequence()
{
    return mString.string();
}

void Text::writeToParcel(Parcel *out, int flags) {
    out->writeString8(mString);
}

int Text::describeContents() {
    return 0;
}

Text *Text::createFromParcel(const Parcel& source){
    String8 str = source.readString8();
    Text *txt = new Text(str);

    return txt;
}

void Text::readFromParcel(const Parcel& source) {
    this->mString =  source.readString8();
}

std::vector<Parcelable *> Text::newArray(int size) {
    std::vector<Parcelable *> v;
    for (int i = 0; i < size; ++i) {
        v.push_back(new Text(""));
    }

    return v;
}


};


