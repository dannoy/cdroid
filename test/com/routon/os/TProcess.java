/*
 * Author:lijin@routon
 * Date:20121225
 */

package com.routon.os;

import com.routon.os.TResult;
import com.routon.os.ITProcess;

import android.util.Log;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;

public class TProcess {
    private static final String TAG = "TProcess";
    private static final TProcess mTProcess = new TProcess();
    private ITProcess mService = null;

    TProcess() {
        IBinder b = ServiceManager.getService("tprocess");
        mService = ITProcess.Stub.asInterface(b);
        //Log.i(TAG, "mService " + mTProcess);
    }

    public static TProcess getTProcess() {
        return mTProcess;
    }

    public TResult exec(String cmd) {
        return exec(cmd, true);
    }

    public TResult exec(String cmd, boolean redirectStderrToStdout) {
        try{
            /* Just in case tprocess start again after died */
            if(mService == null) {
                IBinder b = ServiceManager.getService("tprocess");
                mService = ITProcess.Stub.asInterface(b);
            }
            return mService.exec(cmd, redirectStderrToStdout);
        } catch (RemoteException ex) {
            Log.e(TAG, "RemoteException in exec", ex);
            mService = null;
            return null;
        }
    }
}
