#ifndef _SERVICE_WINDOWMANAGER_H
#define _SERVICE_WINDOWMANAGER_H

#include <service/IWindowManager.h>


namespace cdroid {
    
class WindowManager : public RefBase {
public:
    WindowManager();
    void onKeyEvent(sp<KeyEvent> e);
    int registerFocusWindow(sp<IBinder> win);
    int displayText(sp<Text> txt);

private:
    static sp<IWindowManager> getDefault();
    static sp<IWindowManager> sProxy;
    static Mutex mSingletonMutex;
};


};

#endif
