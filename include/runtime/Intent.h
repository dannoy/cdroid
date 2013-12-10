#ifndef _RUNTIME_INTENT_H
#define _RUNTIME_INTENT_H

#include <runtime/common.h>
namespace cdroid {

class ComponentName : public RefBase {
public:
    ComponentName(String8 name)
        :mName(name)
    {
    }
    ComponentName(ComponentName& comp)
        :mName(comp.mName)
    {
    }
    String8& getName()
    {
        return mName;
    }
    
    bool setName(String8 name)
    {
        mName = name;
        return true;
    }
private:
    String8 mName;
};

class Intent : public RefBase {
public:
    static String8 ACTION_MAIN;

    static String8 CATEGORY_DEFAULT;
    static String8 CATEGORY_LAUNCHER;

public:
    Intent();
    Intent(String8 action);
    Intent(String8 action, String8 category);
    bool setCategory(String8 category);
    bool setComponent(ComponentName comp);

    String8& getAction()
    {
        return mAction;
    }

    String8& getCategory()
    {
        return mCategory;
    }

    ComponentName& getComponent()
    {
        return mComponent;
    }

private:
    String8 mAction;
    String8 mCategory;
    ComponentName mComponent;

};

};

#endif
