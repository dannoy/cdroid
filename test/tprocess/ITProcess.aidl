/*
 * Author:lijin@routon
 * Date:20121225
 */

package com.routon.os;

cinclude{
test
#include <stdint.h>
#include <sys/types.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <ui/Rect.h>
#include <utils/String8.h> 
}cend

import com.routon.os.TResult;

interface ITProcess
{
    /*
     * cmd is a legal command string to shell
     */
    TResult exec(String cmd, boolean redirectStderrToStdout);
}
