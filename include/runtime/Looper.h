#ifndef _RUNTIME_LOOPER_H
#define _RUNTIME_LOOPER_H
#include <runtime/common.h>
#include <utils/Looper.h>
#include <utils/Timers.h>


namespace cdroid {

using android::LooperCallback;

enum {
    /**
     * The file descriptor is available for read operations.
     */
    LOOPER_EVENT_INPUT = ALOOPER_EVENT_INPUT,

    /**
     * The file descriptor is available for write operations.
     */
    LOOPER_EVENT_OUTPUT = ALOOPER_EVENT_OUTPUT,

    /**
     * The file descriptor has encountered an error condition.
     *
     * The looper always sends notifications about errors; it is not necessary
     * to specify this event flag in the requested event set.
     */
    LOOPER_EVENT_ERROR = ALOOPER_EVENT_ERROR,

    /**
     * The file descriptor was hung up.
     * For example, indicates that the remote end of a pipe or socket was closed.
     *
     * The looper always sends notifications about hangups; it is not necessary
     * to specify this event flag in the requested event set.
     */
    LOOPER_EVENT_HANGUP = ALOOPER_EVENT_HANGUP,

    /**
     * The file descriptor is invalid.
     * For example, the file descriptor was closed prematurely.
     *
     * The looper always sends notifications about invalid file descriptors; it is not necessary
     * to specify this event flag in the requested event set.
     */
    LOOPER_EVENT_INVALID = ALOOPER_EVENT_INVALID,
};

class Looper : public android::Looper {
public:
    static void prepare();
    static void prepareMainLooper();
    static sp<Looper> getMainLooper();
    static sp<Looper> myLooper();
    static void loop();
    int addFd(int fd, int events, const sp<LooperCallback>& callback, void* data);

private:
    Looper();
    static void setMainLooper(sp<Looper> l);

    static sp<Looper> mMainLooper;
    Mutex mLock;
};

class Runnable {
public:
    virtual int run() = 0;
};


class Message : public android::Message {
public:
    Message();
    Message(int what);
    Message(sp<Runnable> r);
private:
    sp<Runnable> r;
};

class Handler : public android::MessageHandler {
public:
    Handler();
    virtual void handleMessage(const android::Message& message);
    virtual void handleMessage(const Message& message) = 0;

    void sendMessage(const Message& message);
    void sendMessageDelayed(nsecs_t uptimeDelay, const Message& message);
    void sendMessageAtTime(nsecs_t uptime, const Message& message);
    bool post(const Runnable& r);

private:
    sp<Looper> mLooper;
};


};

#endif
