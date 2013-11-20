/*
 * Author:lijin@routon
 * Date:20121225
 */

package com.routon.os;

import com.routon.os.TResult;

interface ITProcess
{
    /*
     * cmd is a legal command string to shell
     */
    TResult exec(String cmd, boolean redirectStderrToStdout);
}
