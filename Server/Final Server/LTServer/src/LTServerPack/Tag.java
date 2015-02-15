/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */


package LTServerPack;

/**
 * A class storing the properties of a RFID-tag.
 * 
 * @author Marc
 */
public class Tag {
    
    /**
     * Constructor. Initializes all members.
     */
    public Tag() {
        tagNo = -1;
        uid = null;
        uidHexString = "missing";
        name = "missing";
        reloadAmmu = false;
        reloadHealth = false;
        ammuTo = false;
        ammuToValue = -1;
        healthTo = false;
        healthToValue = -1;
        pointsTo = false;
        pointsToValue = -1;
    }
    
    // MEMBERS:
    // The number of the tag.
    public int tagNo;
    // The unique ID of the tag.
    public byte[] uid;
    // The unique ID of the tag as hex sring.
    public String uidHexString;
    // The name/info of the tag.
    public String name;
    // States if the ammunition should be reloaded with this tag.
    public boolean reloadAmmu;
    // States if the health should be reloaded with this tag.
    public boolean reloadHealth;
    // States if the ammunition should be set to ammuToValue with this tag.
    public boolean ammuTo;
    public int ammuToValue;
    // States if the health should be set to healthToValue with this tag.
    public boolean healthTo;
    public int healthToValue;
    // States if the points should be set to pointsToValue with this tag.
    public boolean pointsTo;
    public int pointsToValue;
}
