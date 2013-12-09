#ifndef _RUNTIME_ACTIVITY_H
#define _RUNTIME_ACTIVITY_H

#include <runtime/Context.h>
#include <runtime/Bundle.h>


namespace cdroid {

class Activity : public ContextWrapper {
public:
    virtual void onCreate(Bundle savedInstanceState);
    virtual void onStart();
    virtual void onResume();
    virtual void onPause();
    virtual void onStop();
    virtual void onDestroy();

};

};


#endif
