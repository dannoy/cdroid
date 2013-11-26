/*
 * Author:lijin@routon
 * Date:20121225
 */

#ifndef _TPROCESSSERVICE_H
#define _TPROCESSSERVICE_H

#include "ITProcess_aidl.h"

namespace android
{
    class TProcessService: public BnTProcess
    {
        public:
            TProcessService();
            ~TProcessService();
            static void instantiate();
            virtual TResult exec(String16 cmd, int redirectStderrToStdout);
            int tprocess_exec(String16& cmdStr,
                                            int redirectStderrToStdout,
                                            int *rc, char **out, char **err);
            int tprocess_exec(const char *cmd, int redirectStderrToStdout,
                                            int *rc, char **out, char **err);
        private:
            int _tprocess_daemonize(const char *cmd,
                                            int redirectStderrToStdout,
                                            int *rc, char **out, char **err);
            int _collect_output_by_fd(int fd, char **out);
            int _talk_and_collect(const char *cmd, int redirectStderrToStdout,
                                            int *rc, char **out, char **err,
                                            int inpipe[], int outpipe[], int errpipe[]);
            int _fork_and_exec(const char *cmd, int redirectStderrToStdout,
                                            int *rc, char **out, char **err,
                                            int inpipe[], int outpipe[], int errpipe[]);
    };
};

#endif
