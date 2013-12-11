#define LOG_TAG "DisplayManager"
#include <time.h>

#include <cutils/log.h>

#include <binder/IServiceManager.h>
#include <service/DisplayManager.h>


namespace cdroid {
sp<IDisplayManager> DisplayManager::sProxy;
Mutex DisplayManager::mSingletonMutex;

DisplayManager::DisplayManager()
{
}

sp<IDisplayManager> DisplayManager::getDefault()
{
    if(!sProxy.get()) {
        AutoMutex _l(mSingletonMutex);
        if(!sProxy.get()) {
            sp<IServiceManager> sm = android::defaultServiceManager();
            sp<IBinder> service = sm->checkService(String16("display"));

            while(service == NULL) {
                struct timespec t = {0, 50000000};
                nanosleep(&t, NULL);
                service = sm->checkService(String16("display"));
            }

            sProxy = IDisplayManager::asInterface(service);
            assert(sProxy.get() != NULL);
        }
    }
    return sProxy;
}


int DisplayManager::displayText(sp<Text> txt)
{
    return getDefault()->displayText(txt);
}



};
