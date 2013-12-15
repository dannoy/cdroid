#define LOG_TAG "IntentFilter"
#include <cutils/log.h>

#include <runtime/IntentFilter.h>

namespace cdroid {

IntentFilter::IntentFilter() 
    : mPriority(0)
{
}

IntentFilter::IntentFilter(String8 action)
{
    mActions.push_back(action);
}

IntentFilter::IntentFilter(sp<IntentFilter> inf)
{
    for(Vector<String8>::iterator it = inf->mActions.begin(); it != inf->mActions.end(); ++it) {
        mActions.push_back(*it);
    }

    for(Vector<String8>::iterator it = inf->mCategories.begin(); it != inf->mCategories.end(); ++it) {
        mCategories.push_back(*it);
    }
}

void IntentFilter::addAction(String8 action)
{
    mActions.push_back(action);
}

void IntentFilter::addCategory(String8 category)
{
    mCategories.push_back(category);
}

bool IntentFilter::matchIntent(sp<Intent> intent)
{
    for(Vector<String8>::iterator it = mActions.begin(); it != mActions.end(); ++it) {
        if(*it == intent->getAction()) {
            return true;
        }
    }
    
    return false;
}

int IntentFilter::describeContents() {
    return 0;
}

void IntentFilter::writeToParcel(Parcel *out, int flags) {
    int N = mActions.size();
    out->writeInt32(N);
    for(Vector<String8>::iterator it = mActions.begin(); it != mActions.end(); ++it) {
        out->writeString8(*it);
    }

    N = mCategories.size();
    out->writeInt32(N);
    for(Vector<String8>::iterator it = mCategories.begin(); it != mCategories.end(); ++it) {
        out->writeString8(*it);
    }
}

IntentFilter *IntentFilter::createFromParcel(const Parcel& source){
    IntentFilter *inf = new IntentFilter();
    int N = source.readInt32();
    for(int i = 0; i < N; ++i) {
        inf->mActions.push_back(source.readString8());
    }

    N = source.readInt32();
    for(int i = 0; i < N; ++i) {
        inf->mCategories.push_back(source.readString8());
    }

    return inf;
}

void IntentFilter::readFromParcel(const Parcel& source) {
    int N = source.readInt32();
    for(int i = 0; i < N; ++i) {
        mActions.push_back(source.readString8());
    }

    N = source.readInt32();
    for(int i = 0; i < N; ++i) {
        mCategories.push_back(source.readString8());
    }
}

std::vector<Parcelable *> IntentFilter::newArray(int size) {
    std::vector<Parcelable *> v;
    for (int i = 0; i < size; ++i) {
        v.push_back(new IntentFilter());
    }

    return v;
}

};
