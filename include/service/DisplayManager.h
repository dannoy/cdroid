#ifndef _SERVICE_DISPLAYMANAGER_H
#define _SERVICE_DISPLAYMANAGER_H

#include <service/IDisplayManager.h>


namespace cdroid {
    
class DisplayManager : public RefBase {
public:
    DisplayManager();
    int displayText(sp<Text> txt);

private:
    static sp<IDisplayManager> getDefault();
    static sp<IDisplayManager> sProxy;
    static Mutex mSingletonMutex;
};


};

#endif
