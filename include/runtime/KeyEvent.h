#ifndef _RUNTIME_KEYEVENT_H
#define _RUNTIME_KEYEVENT_H

#include <runtime/common.h>
#include <runtime/Looper.h>

namespace cdroid {

class KeyEvent : public Parcelable {
public:
    KeyEvent();
    KeyEvent(int code, int action);

    void setCode(int code) {
        mCode = code;
    }

    void setAction(int action) {
        mAction = action;
    }

    int describeContents();
    void writeToParcel(Parcel *out, int flags);
    KeyEvent *createFromParcel(const Parcel& source);
    void readFromParcel(const Parcel& source);
    std::vector<Parcelable *> newArray(int size);

    int& getCode() {
        return mCode;
    }

    int& getAction() {
        return mAction;
    }

    enum {
        KEYEVENT_UP = 1,
        KEYEVENT_DOWN
    };
private:
    int mCode;
    int mAction;

};

};

#endif
