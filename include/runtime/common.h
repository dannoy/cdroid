#ifndef CDROIDRUNTIME_COMMON_H
#define CDROIDRUNTIME_COMMON_H

#include <utils/RefBase.h>
#include <utils/Mutex.h>
#include <utils/Vector.h>
#include <utils/String8.h>
#include <utils/Thread.h>
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/Parcel.h>

namespace cdroid {
    using android::sp;
    using android::wp;
    using android::Mutex;
    using android::AutoMutex;
    using android::Vector;
    using android::String8;
    using android::RefBase;
    using android::IInterface;
    using android::IBinder;
    using android::BnInterface;
    using android::BpInterface;
    using android::Parcel;
    using android::Thread;

template <class T>
    class Singleton
    {
        public:
            static sp<T> Instance() {
                if(!m_pInstance.get()) m_pInstance = new T;
                assert(m_pInstance.get() != NULL);
                return m_pInstance;
            }
        protected:
            Singleton();
            ~Singleton();
        private:
            Singleton(Singleton const&);
            Singleton& operator=(Singleton const&);
            static sp<T> m_pInstance;
    };

template <class T> sp<T> Singleton<T>::m_pInstance=NULL;

};


#endif
