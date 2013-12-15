#ifndef _RUNTIME_INTENTFILTER_H
#define _RUNTIME_INTENTFILTER_H

#include <runtime/common.h>
#include <runtime/Intent.h>
namespace cdroid {

class IntentFilter : public Parcelable {
public:
    IntentFilter();
    IntentFilter(String8 action);
    IntentFilter(sp<IntentFilter> inf);

    void addAction(String8 action);
    void addCategory(String8 action);

    bool matchIntent(sp<Intent> intent);

    int describeContents();
    void writeToParcel(Parcel *out, int flags);
    IntentFilter *createFromParcel(const Parcel& source);
    void readFromParcel(const Parcel& source);
    std::vector<Parcelable *> newArray(int size);
    

private:
    Vector<String8> mActions;
    Vector<String8> mCategories;
    int mPriority;
};


};

#endif
