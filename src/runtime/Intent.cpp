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

};
