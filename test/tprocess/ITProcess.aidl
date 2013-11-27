/*
 * Author:lijin@routon
 * Date:20121225
 */

package com.routon.os;

%%{
#include <stdint.h>
#include <sys/types.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Binder.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
}%%

import com.routon.os.TResult;

interface ITProcess
{
    /*
     * cmd is a legal command string to shell
     */
    TResult exec(String cmd, boolean redirectStderrToStdout);
}
