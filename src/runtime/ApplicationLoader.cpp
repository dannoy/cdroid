#define LOG_NDEBUG 1

#define LOG_TAG "Application"
#include <cutils/log.h>
#include <dlfcn.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>


#define APPLICATION_LIBRARY_PATH "lib"

#include <runtime/ApplicationLoader.h>

namespace cdroid {


ApplicationLoader::ApplicationLoader()
{
}

int handleFile(const char *filename, ApplicationManifest **am)
{
    int status;
    void *handle;
    struct ApplicationManifest *pam = NULL;
    const char *sym = APPLICATION_INFO_SYM_AS_STR;

    /*
     * load the symbols resolving undefined symbols before
     * dlopen returns. Since RTLD_GLOBAL is not or'd in with
     * RTLD_NOW the external symbols will not be global
     */
    handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    if (handle == NULL) {
        char const *err_str = dlerror();
        ALOGE("load: module=%s\n%s", filename, err_str?err_str:"unknown");
        status = -EINVAL;
        goto done;
    }

    /* Get the address of the struct hal_module_info. */
    pam = (struct ApplicationManifest *)dlsym(handle, sym);
    if (pam == NULL) {
        //ALOGE("load: couldn't find symbol %s in %s", sym, filename);
        status = -EINVAL;
        goto done;
    }

    pam->dso = handle;

    /* success */
    status = 0;

    done:
    if (status != 0) {
        am = NULL;
        if (handle != NULL) {
            dlclose(handle);
            handle = NULL;
        }
    } else {
        *am = pam;
        ALOGV("loaded Application name=%s path=%s ai=%p handle=%p",
                pam->name, filename, *pam, handle);
    }


    return status;
}


int ApplicationLoader::loadApplications(sp<ApplicationLoaderCallback> callback)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(APPLICATION_LIBRARY_PATH);
    if(dir == NULL)
        return -1;
    strcpy(devname, APPLICATION_LIBRARY_PATH);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
        ApplicationManifest *app = NULL;
        handleFile(filename, &app);

        if(app) {
            callback->onApplication(app, String8(devname));
        }
    }
    closedir(dir);
    return 0;
}

ActivityLoader::ActivityLoader()
{
}

ActivityManifest* ActivityLoader::loadActivity(String8 filename, String8 activityName)
{
    ApplicationManifest *app = NULL;
    handleFile(filename, &app);

    if(app) {
        ActivityManifest *am = app->activity;
        while(am) {
            if(am->name == activityName){
                return am;
            }
        }
    }

    return NULL;
}


ServiceLoader::ServiceLoader()
{
}

ServiceManifest* ServiceLoader::loadService(String8 filename, String8 serviceName)
{
    ApplicationManifest *app = NULL;
    handleFile(filename, &app);

    if(app) {
        ServiceManifest *sm = app->service;
        while(sm) {
            if(sm->name == serviceName){
                return sm;
            }
        }
    }

    return NULL;
}

};
