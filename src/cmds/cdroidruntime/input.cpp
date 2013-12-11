#define LOG_TAG "CDroidRuntime"
#include <utils/Log.h>
#include "input.h"
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>

#include "service/WindowManager.h"
//#include "DisplayManagerService.h"


namespace cdroid {

#define TC_VTIME 1
#define TC_MIN_CHAR 20

//#define TCFLAGS (ECHO | ECHOE | ECHOK | ECHONL | ICANON | ISIG)
#define TCFLAGS (ECHO | ECHOE | ECHOK | ECHONL | ICANON)

static int _set_disp_mode()
{
    int err;
    struct termios term;
    struct termios newterm;
    if(tcgetattr(0,&term)==-1){
        perror("Cannot get the attribution of the terminal");
        return 1;
    }
    /* ICANON取反，表示关闭输入行编辑模式，这样我们可以直接read字符，不用等着敲回车.
     * ISIG取反，表示禁止信号，这样Crtl+c Crtl+s 就不会产生信号了，当你发现用户键入
     * Crtl+c后，你可以自定义一个动作
     */
    newterm = term;
    newterm.c_cc[VTIME] = TC_VTIME;//终端等待时间(以十分之一秒为单位)
    newterm.c_cc[VMIN] = TC_MIN_CHAR;//终端接收字符个数（或的关系）
    //终端等待5个单位时间或者接收字符达到20个
    newterm.c_lflag &= ~TCFLAGS;
    err=tcsetattr(0,TCSAFLUSH,&newterm);
    if(err==-1 && err==EINTR){
        perror("Cannot set the attribution of the terminal");
        return 1;
    }
    return 0;
}

int input_init()
{
    _set_disp_mode();
    char buf[TC_MIN_CHAR];
    WindowManager wm;
    sp<KeyEvent> e = new KeyEvent(-1, -1);
    while(1) {
        int n = read(0, buf, sizeof(buf));
        for(int i = 0; i < n; ++i) {
            //ALOGD("char (%c:%x)", buf[i], buf[i]);
            //fprintf(stderr,"char (%c:%x)\n", buf[i], buf[i]);
            //printf("char (%c:%x)\n", buf[i], buf[i]);
            e->setCode(buf[i]);
            wm.onKeyEvent(e);
        }
    };
}

int output_init()
{
    ALOGD("output_init");
    //DisplayManagerService::instantiate();
    ALOGD("output_init over");
    return 0;
}
}
