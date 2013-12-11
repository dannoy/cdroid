#define LOG_TAG "ActivityStack"
#include <cutils/log.h>

#include <runtime/Intent.h>

namespace cdroid {

String8 Intent::ACTION_MAIN("cdroid::intent::action::main");

String8 Intent::CATEGORY_DEFAULT("cdroid::intent::category::default");
String8 Intent::CATEGORY_LAUNCHER("cdroid::intent::category::launcher");

Intent::Intent()
    : mAction(""),
    mCategory(""),
    mComponent(String8(""))
{
}

Intent::Intent(String8 action)
    : mAction(action),
    mCategory(""),
    mComponent(String8(""))
{
}

Intent::Intent(String8 action, String8 category)
    : mAction(action),
    mCategory(category),
    mComponent(String8(""))
{
}

bool Intent::setCategory(String8 category)
{
    mCategory = category;
}
bool Intent::setComponent(ComponentName comp)
{
    mComponent.setName(comp.getName());
}

int Intent::describeContents() {
    return 0;
}

void Intent::writeToParcel(Parcel *out, int flags) {
    out->writeString8(mAction);
    out->writeString8(mCategory);
}

Intent *Intent::createFromParcel(const Parcel& source){
    String8 action = source.readString8();
    String8 category = source.readString8();

    Intent *i = new Intent(action, category);

    return i;
}

void Intent::readFromParcel(const Parcel& source) {
    this->mAction =  source.readString8();
    this->mCategory =  source.readString8();
}

std::vector<Parcelable *> Intent::newArray(int size) {
    std::vector<Parcelable *> v;
    for (int i = 0; i < size; ++i) {
        v.push_back(new Intent(String8(""),String8("")));
    }

    return v;
}

};
