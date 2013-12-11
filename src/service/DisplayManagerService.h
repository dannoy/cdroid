#ifndef _SERVICE_DISPLAYMANAGER_SERVICE_H
#define _SERVICE_DISPLAYMANAGER_SERVICE_H

#include <runtime/common.h>
#include <runtime/Looper.h>
#include <service/IDisplayManager.h>

namespace cdroid {

class DisplayManagerService : public BnDisplayManager {
public:
    DisplayManagerService();
    static void instantiate();
    /*void handleDisplayTextLocked(sp<Text> txt);*/

    // interfaces
    /*virtual int displayText(sp<Text> txt);*/

    /*private:*/
    /*class H : public Handler {*/
    /*public:*/
    /*H(sp<DisplayManagerService> disp, sp<Looper> looper)*/
    /*: mDisp(disp),*/
    /*Handler(looper)*/
    /*{*/
    /*}*/
    /*enum {*/
    /*DISPLAY_TEXT = 1,*/
    /*};*/
    /*virtual void handleMessage(const sp<Message>& message);*/
    /*private:*/
    /*sp<DisplayManagerService> mDisp;*/
    /*};*/

    /*private:*/
    /*sp<Looper> mLooper;*/
    /*sp<Handler> mH;*/
};

};

#endif
