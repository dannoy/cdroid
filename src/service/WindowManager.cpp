#define LOG_TAG "WindowManager"
#include <cutils/log.h>

#include <binder/IServiceManager.h>
#include <service/WindowManager.h>


namespace cdroid {
sp<IWindowManager> WindowManager::sProxy;
Mutex WindowManager::mSingletonMutex;

WindowManager::WindowManager()
{
}

sp<IWindowManager> WindowManager::getDefault()
{
    if(!sProxy.get()) {
        AutoMutex _l(mSingletonMutex);
        if(!sProxy.get()) {
            sp<IServiceManager> sm = android::defaultServiceManager();
            sp<IBinder> const service = sm->checkService(String16("window"));

            sProxy = IWindowManager::asInterface(service);
            assert(sProxy.get() != NULL);
        }
    }
    return sProxy;
}


void WindowManager::onKeyEvent(sp<KeyEvent> e)
{
    getDefault()->onKeyEvent(e);
}

int WindowManager::registerFocusWindow(sp<IBinder> win)
{
    return getDefault()->registerFocusWindow(win);
}

int WindowManager::displayText(sp<Text> txt)
{
    return getDefault()->displayText(txt);
}



};
