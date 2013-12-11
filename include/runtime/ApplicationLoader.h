#ifndef _RUNTIME_APPLICATION_H
#define _RUNTIME_APPLICATION_H

#include <runtime/Activity.h>


namespace cdroid {

struct ActivityManifest {
        String8               name;
        String8               action;
        String8               category;
        ActivityManifest    *next;
        Activity*           (*createActivity)(sp<Intent> intent);
};

struct ApplicationManifest {
    String8 name;
    // shared lib handle
    void *dso;
    ActivityManifest *activity;
};

class ApplicationLoaderCallback : public RefBase{
public:
    virtual int onApplication(struct ApplicationManifest *appM, String8 file) = 0;
};

class ApplicationLoader : public RefBase{
public:
    ApplicationLoader();
    int loadApplications(sp<ApplicationLoaderCallback> callback);
private:
};

class ActivityLoader : public RefBase{
public:
    ActivityLoader();
    static ActivityManifest* loadActivity(String8 filename, String8 activityName);
private:
};

#define APPLICATION_INFO_SYM appinfo
#define APPLICATION_INFO_SYM_AS_STR "appinfo"


};

#endif
