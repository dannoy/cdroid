#ifndef _RUNTIME_ACTIVITY_INFO_H
#define _RUNTIME_ACTIVITY_INFO_H

#include <runtime/common.h>

namespace cdroid {

class ActivityInfo : public RefBase {
public:
    ActivityInfo();
private:
    String8 name;
    String8 filename;
};



};

#endif
