
/**
 * Interface for classes whose instances can be written to
 * and restored from a {@link Parcel}.  Classes implementing the Parcelable
 * interface must also have a static field called <code>CREATOR</code>, which
 * is an object implementing the {@link Parcelable.Creator Parcelable.Creator}
 * interface.
 *
 * <p>A typical implementation of Parcelable is:</p>
 *
 * <pre>
 * class MyParcelable : public Parcelable {
 *     private int mData;
 *
 *     public int describeContents() {
 *         return 0;
 *     }
 *
 *     public void writeToParcel(Parcel out, int flags) {
 *         out.writeInt(mData);
 *     }
 *
 *     public MyParcelable createFromParcel(Parcel in) {
 *         return new MyParcelable(in);
 *     }
 *
 *     public MyParcelable[] newArray(int size) {
 *         return new MyParcelable[size];
 *     }
 *
 *     private MyParcelable(Parcel in) {
 *         mData = in.readInt();
 *     }
 * }</pre>
 */
namespace android{
class Parcelable {
    public:
    /**
     * Flag for use with {@link #writeToParcel}: the object being written
     * is a return value, that is the result of a function such as
     * "<code>Parcelable someFunction()</code>",
     * "<code>void someFunction(out Parcelable)</code>", or
     * "<code>void someFunction(inout Parcelable)</code>".  Some implementations
     * may want to release resources at this point.
     */
    static final int PARCELABLE_WRITE_RETURN_VALUE = 0x0001;

    /**
     * Bit masks for use with {@link #describeContents}: each bit represents a
     * kind of object considered to have potential special significance when
     * marshalled.
     */
    static final int CONTENTS_FILE_DESCRIPTOR = 0x0001;

    /**
     * Describe the kinds of special objects contained in this Parcelable's
     * marshalled representation.
     *
     * @return a bitmask indicating the set of special object types marshalled
     * by the Parcelable.
     */
    int describeContents();

    /**
     * Flatten this object in to a Parcel.
     *
     * @param dest The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     * May be 0 or {@link #PARCELABLE_WRITE_RETURN_VALUE}.
     */
    void writeToParcel(Parcel& dest, int flags) = 0;

    /**
     * Create a new instance of the Parcelable class, instantiating it
     * from the given Parcel whose data had previously been written by
     * {@link Parcelable#writeToParcel Parcelable.writeToParcel()}.
     *
     * @param source The Parcel to read the object's data from.
     * @return Returns a new instance of the Parcelable class.
     */
    Parcelable createFromParcel(Parcel& source) = 0;

    /**
     * Create a new array of the Parcelable class.
     *
     * @param size Size of the array.
     * @return Returns an array of the Parcelable class, with every entry
     * initialized to null.
     */
    Parcelable[] newArray(int size) = 0;
};
};
