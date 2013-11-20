/*
 * Author:lijin@routon
 * Date:20121225
 */

package com.routon.os;

import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

public class TResult implements Parcelable {
    private int mStatus = 0;
    private String mStdoutStr = null;
    private String mStderrStr = null;

    public TResult(int rc, String stdout, String stderr) {
        mStatus = rc;
        mStdoutStr = stdout;
        mStderrStr = stderr;
    }

    public TResult(Parcel in) {
        mStatus = in.readInt();
        mStdoutStr = in.readString();
        mStderrStr = in.readString();
    }

    public int getStatus() {
        return mStatus;
    }

    public String getStdoutString() {
        return mStdoutStr;
    }

    public String getStderrString() {
        return mStderrStr;
    }

    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mStatus);
        out.writeString(mStdoutStr);
        out.writeString(mStderrStr);
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("TResult: mStatus("); sb.append(mStatus); sb.append(") ");
        sb.append("mStdoutStr("); sb.append(mStdoutStr); sb.append(") ");
        sb.append("mStderrStr("); sb.append(mStderrStr); sb.append(") ");
        return sb.toString();
    }

    public static final Parcelable.Creator<TResult> CREATOR =
            new Parcelable.Creator<TResult>() {
        public TResult createFromParcel(Parcel in) {
            return new TResult(in);
        }
        public TResult[] newArray(int size) {
            return new TResult[size];
        }
    };
}
