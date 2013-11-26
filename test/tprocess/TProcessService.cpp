/*
 * Author:lijin@routon
 * Date:20121225
 */

#define ALOG_TAG "TProcessService"

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <utils/String16.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "TProcessService.h"

namespace android
{
    TProcessService::TProcessService()
    {
    }

    TProcessService::~TProcessService()
    {
    }

    void TProcessService::instantiate()
    {
        defaultServiceManager()->addService(String16("tprocess"), new TProcessService());
    }

    int TProcessService::_collect_output_by_fd(int fd, char **out) {
        FILE *rp = NULL;
        char line[BUFSIZ];
        int len = 0;
        char *outp = NULL;
        int stepsize = BUFSIZ;
        int curlen = 0, curpos = 0;
        int ret = 0;

        if(NULL == (rp = fdopen(fd, "r"))){
            ALOGE("Error fdopen \n");
            return -1;
        }
        else if(NULL == (outp = (char *)malloc(stepsize * sizeof(char)))) {
            ALOGE("Out Of Memory!!!\n");
            return -1;
        }
        outp[0] = '\0';
        curlen = stepsize;
        /* For '\0' */
        curpos = 1;
        /*
         * The output is expected end with newline.
         * Sadly, there is no getline() in bionic, so I use fgets instead.
         */
        while (NULL != fgets(line, BUFSIZ, rp)) {
            len = strlen(line);
            if((curlen - curpos) < len + 1) {
                if(NULL == (outp = (char *)realloc(outp, curlen + stepsize))) {
                    ALOGE("Out Of Memory!!!\n");
                    *out = NULL;
                    ret = -1;
                    goto outl;
                }
                curlen += stepsize;
            }
            memcpy(outp + curpos - 1, line, len + 1);
            curpos += len;
        }

        *out = outp;

outl:
        return ret;
    }

    int TProcessService::_talk_and_collect(const char *cmd, int redirectStderrToStdout,
                                    int *rc, char **out, char **err,
                                    int inpipe[], int outpipe[], int errpipe[]) {
        int cmd_len = strlen(cmd);
        int ret = 0;

        do{
            ret = write(inpipe[1], cmd, cmd_len);
        }while(-1 == ret && errno == EINTR);
        /*
         * Close talk fds
         * After close inpipe[1], shell program should exit automatically
         * */
        close(inpipe[1]);

        if(redirectStderrToStdout) {
            *err = NULL;
        }
        else {
            ret = _collect_output_by_fd(errpipe[0], err);
            //ALOGD("stderr output:%s ret %d", *err, ret);
        }

        ret = _collect_output_by_fd(outpipe[0], out);
        //ALOGD("stdout output:%s ret %d", *out, ret);

        return ret;
    }

    int TProcessService::_fork_and_exec(const char *cmd, int redirectStderrToStdout,
                                    int *rc, char **out, char **err,
                                    int inpipe[], int outpipe[], int errpipe[]) {
        pid_t cpid;
        int status = 0;
        int ret = 0;

        if((cpid = fork()) < 0) {
            ALOGE("Fork failed:%s!\n", strerror(errno));
            ret = cpid;
            goto outl;
        }
        else if(cpid == 0) {

            dup2(inpipe[0], 0);
            dup2(outpipe[1], 1);
            if(redirectStderrToStdout) {
                dup2(outpipe[1], 2);
            }
            else {
                dup2(errpipe[1], 2);
            }

            /*
             * Close unneeded file descriptors
             */
            close(inpipe[0]);
            close(inpipe[1]);
            close(outpipe[0]);
            close(outpipe[1]);
            if(!redirectStderrToStdout) {
                close(errpipe[0]);
                close(errpipe[1]);
            }

            if((ret = execlp("sh", "sh", NULL)) < 0) {
                ALOGE("Exec failed:%s!\n", strerror(errno));
                return -1;
            }
        }

        /*
         * Close unneeded file descriptors
         */
        close(inpipe[0]);
        close(outpipe[1]);
        if(!redirectStderrToStdout) {
            close(errpipe[1]);
        }

        ret = _talk_and_collect(cmd, redirectStderrToStdout,
                                    rc, out, err,
                                    inpipe, outpipe, errpipe);

        close(outpipe[0]);
        if(!redirectStderrToStdout) {
            close(errpipe[0]);
        }
        if((ret = waitpid(cpid, &status, 0)) < 0) {
            ALOGE("Waitpid failed:%s!\n", strerror(errno));
        }
        else {
            *rc = WEXITSTATUS(status);
        }
        ret = 0;

outl:
        return ret;
    }

    void* _wait_daemon(void *arg) {
        int ret = 0;
        int status;
        pid_t pid = (pid_t)arg;

        if((ret = waitpid(pid, &status, 0)) < 0) {
            ALOGE("DAE:Waitpid failed:%s!\n", strerror(errno));
        }
        else {
            ret = WEXITSTATUS(status);
            ALOGE("DAE %d exit status %d!\n", pid, ret);
        }

        return NULL;
    }

    int TProcessService::_tprocess_daemonize(const char *cmd, int redirectStderrToStdout,
                                            int *rc, char **out, char **err) {
        int handled = 0;
        int pid = -1;
        pthread_t tid;
        char *exe[50];
        char *p = NULL;
        int i = 0;

        memset(exe, 0, sizeof(exe));

        if(0 != strncmp(cmd, "daemonize", 9)) {
            return handled;
        }
        ALOGD("DAE found");
        p = (char *)cmd + 9;
        p[strlen(p) - 1] = 0;

        for(i = 0; i < sizeof(exe); ++i) {
           while(*p == ' ') ++p;
           if(p) {
               exe[i] = p;
               if(NULL != (p = strchr(p, ' '))) {
                   *p = '\0';
                   p++;
               }
           }

           if(!p) {
               break;
           }
        }

        //for(i = 0; exe[i] != NULL; ++i) {
            //ALOGD("dae [%d] %s",i, exe[i]);
        //}

        handled = 1;
        switch(pid = fork()) {
          case -1:
            ALOGE("DAE:fork failed");
            exit(1);
            break;
          case 0:
              execv(exe[0], exe);
              execvp(exe[0], exe);
              ALOGE("EXEC failed");
              exit(1);
            break;
          default:
            pthread_create(&tid, NULL, _wait_daemon, (void *)pid);
        }

        return handled;
    }

    int TProcessService::tprocess_exec(const char *cmd, int redirectStderrToStdout,
                                            int *rc, char **out, char **err) {
        int ret = 0;

        //if(_tprocess_daemonize(cmd, redirectStderrToStdout, rc, out, err)) {
            //return ret;
        //}

        int inpipe[2], outpipe[2], errpipe[2];

        if((ret = pipe(inpipe)) < 0) {
            ALOGE("Pipe failed:%s!\n", strerror(errno));
            goto err0;
        }
        else if((ret = pipe(outpipe)) < 0) {
            ALOGE("Pipe failed:%s!\n", strerror(errno));
            goto err1;
        }
        else if(!redirectStderrToStdout
                    && (ret = pipe(errpipe)) < 0) {
            ALOGE("Pipe failed:%s!\n", strerror(errno));
            goto err2;
        }
        else if((ret = _fork_and_exec(cmd, redirectStderrToStdout,
                                        rc, out, err,
                                        inpipe, outpipe, errpipe)) < 0) {
            goto err3;
        }

        return ret;

err3:
        close(errpipe[0]);
        close(errpipe[1]);
err2:
        if(!redirectStderrToStdout) {
            close(outpipe[0]);
            close(outpipe[1]);
        }
err1:
        close(inpipe[0]);
        close(inpipe[1]);
err0:
        return ret;
    }

    int TProcessService::tprocess_exec(String16& cmdStr, int redirectStderrToStdout,
                                            int *rc, char **out, char **err) {
        String8 str8(cmdStr);
        const char *cmd = str8.string();
        ALOGD("exec(%s,%d)\n", cmd, redirectStderrToStdout);

        if(cmd[str8.size()-1] != '\n') {
            str8 += String8("\n");
            const char *cmd2 = str8.string();
            return tprocess_exec(cmd2, redirectStderrToStdout, rc, out, err);;
        }

        return tprocess_exec(cmd, redirectStderrToStdout, rc, out, err);;
    }

    TResult TProcessService::exec(String16 cmd, int redirectStderrToStdout) {
        char *this_stdout = NULL;
        char *this_stderr = NULL;
        const char *space = "";
        int rc = 0;
        int ret = 0;

        ret = tprocess_exec(cmd, redirectStderrToStdout,
                                &rc, &this_stdout, &this_stderr);

        if(0 == rc && 0 != ret) {
            rc = ret;
        }

        TResult tRes(rc,
                    this_stdout ? String16(this_stdout) : String16(),
                    this_stderr ? String16(this_stderr) : String16());

        if(this_stdout) {
            free(this_stdout);
        }
        if(this_stderr) {
            free(this_stderr);
        }

        return tRes;
    }
};
