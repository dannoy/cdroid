#ifndef _RUNTIME_APPLICATION_THREAD_H
#define _RUNTIME_APPLICATION_THREAD_H

#include <runtime/common.h>

namespace cdroid{

class IApplicationThread : public IInterface{
public:
    DECLARE_META_INTERFACE(ApplicationThread);
};

class BnApplicationThread : public BnInterface<IApplicationThread> {
public:
};

};

#endif
