#ifndef _RUNTIME_APPLICATION_H
#define _RUNTIME_APPLICATION_H

#include <runtime/Activity.h>


namespace cdroid {

struct ActivityManifest {
        char*               name;
        ActivityManifest    *next;
        Activity*           (*createActivity)();
};

struct ApplicationManifest {
    char *name;
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
    int handleFile(const char *filename, ApplicationManifest **am);
};

#define APPLICATION_INFO_SYM appinfo
#define APPLICATION_INFO_SYM_AS_STR "_ZL7appinfo"


};

#endif
