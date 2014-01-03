
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <netdb.h>
#include <cerrno>
#include <sys/socket.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <iostream>
#include <libgen.h>
#include <time.h>
#include <unistd.h>

#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <testUtil.h>
#include <values.h>
#include <string.h>
#include <sys/msg.h>

using namespace std;

struct msg_buf{
    int mtype;//消息类型
    int data[2];
};

int test_msg_latency(unsigned int iterations, float iterDelay)
{
    int size = sizeof(unsigned int);
    double min = FLT_MAX, max = 0.0, total = 0.0; // Time in seconds for all

    ssize_t len;
    size_t sofar;

    int yes = 1;
    int ret;
        key_t key;
        //获取key值
        key = ftok(".",  'a');
    if (!fork()) {  /* child */

        int msgid;
        int ret;
         struct msg_buf msgbuf;
         struct msg_buf msgbuf2;
        //创建消息队列
        msgid = msgget(key,  IPC_CREAT|0666);/*通过文件对应*/
        if(msgid == -1)
        {
            printf("creat error\n");
            return -1;
        }

        //以当前进程类型，非阻塞方式发送"test data"到消息队列

        msgbuf.mtype = 1;
        msgbuf2.mtype = 3;
        int val1, val2;
        int res;
        for (unsigned int iter = 0; iter < iterations; iter++) {


            //以非阻塞方式接收数据
            memset(&msgbuf,  0,  sizeof(msgbuf));
            //cout << "client recv0" <<endl;
            ret = msgrcv(msgid,  &msgbuf,  sizeof(msgbuf.data), 1,  0);
            //cout << "client recv" <<endl;
            if(ret == -1)
            {
                perror("child receive message err");
                return -1;
            }

            msgbuf2.data[0] = msgbuf.data[0] + msgbuf.data[1];
            ret = msgsnd(msgid,  &msgbuf2,  size,  0);
            if(ret == -1)
            {
                printf("send message err\n");
                return -1;
            }
        }
            exit(0);
    } else { /* parent */

        //sleep(1);
        int msgid;
        int ret;
         struct msg_buf msgbuf;
         struct msg_buf msgbuf2;
        //创建消息队列
        msgid = msgget(key,  IPC_CREAT|0666);/*通过文件对应*/
        if(msgid == -1)
        {
            printf("creat error\n");
            return -1;
        }

        //以当前进程类型，非阻塞方式发送"test data"到消息队列

        msgbuf.mtype = 1;
        msgbuf2.mtype = 3;


        for (unsigned int iter = 0; iter < iterations; iter++) {
            int val1 = iter;
            int val2 = iter + 3;
            int expected = val1 + val2;  // Expect to get the sum back
            int res;
            msgbuf.data[0] = val1;
            msgbuf.data[1] = val2;
            struct timespec start;
            clock_gettime(CLOCK_MONOTONIC, &start);
            //以非阻塞方式接收数据
            ret = msgsnd(msgid,  &msgbuf,  sizeof(msgbuf.data),  0);
            if(ret == -1)
            {
                printf("send message err\n");
                return -1;
            }
            ret = msgrcv(msgid,  &msgbuf2,  size, 3,  0);
            if(ret == -1)
            {
                perror("parent receive message err");
                return -1;
            }

            res = msgbuf2.data[0];

            struct timespec current;
            clock_gettime(CLOCK_MONOTONIC, &current);
            struct timespec deltaTimespec = tsDelta(&start, &current);
            double delta = ts2double(&deltaTimespec);
            min = (delta < min) ? delta : min;
            max = (delta > max) ? delta : max;
            total += delta;
            int result = res;
            //cout << res <<endl;
            if (result != (int) (iter + iter + 3)) {
                cerr << "Unexpected result for iteration " << iter << endl;
                cerr << "  result: " << result << endl;
                cerr << "expected: " << expected << endl;
            }

            if (iterDelay > 0.0) { testDelaySpin(iterDelay); }

        }


    }
    cout << "msg Time per iteration min: " << min
        << " avg: " << (total / iterations)
        << " max: " << max
        << " total: " << total
        << endl;

    return 0;
}
