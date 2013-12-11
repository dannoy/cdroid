#define LOG_TAG "KeyEvent"
#include <cutils/log.h>

#include <runtime/KeyEvent.h>

namespace cdroid {

KeyEvent::KeyEvent()
    : mCode(-1),
    mAction(-1)
{
}

KeyEvent::KeyEvent(int code, int action)
    : mCode(code),
    mAction(action)
{
}

void KeyEvent::writeToParcel(Parcel *out, int flags)
{
    out->writeInt32(mCode);
    out->writeInt32(mAction);
}

int KeyEvent::describeContents() {
    return 0;
}

KeyEvent *KeyEvent::createFromParcel(const Parcel& source)
{

    KeyEvent *e = new KeyEvent(source.readInt32(), source.readInt32());

    return e;
}

void KeyEvent::readFromParcel(const Parcel& source) {
    this->mCode =  source.readInt32();
    this->mAction =  source.readInt32();
}

std::vector<Parcelable *> KeyEvent::newArray(int size) {
    std::vector<Parcelable *> v;
    for (int i = 0; i < size; ++i) {
        v.push_back(new KeyEvent(-1,-1));
    }

    return v;
}

};
