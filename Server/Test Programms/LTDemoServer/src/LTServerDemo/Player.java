/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */

package LTServerDemo;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.net.Socket;

/**
 * Class that the represents a laser tag player including all his stats, his
 * tagger and vest number and the connection to him.
 * 
 * @author Marc
 */
public class Player {
    
    /**
     * Constructor. Initializes all members.
     */
    public Player() {
        health = -1;
        westNr = -1;
        playerNr = -1;
        name = "missing";
        team = "missing";
        conToPlayer = null;
        outToPlayer = null;
        inFromPlayer = null;
    }
    
    //MEMBERS:
    // The current helath of the player.
    public int health;
    // The number of the player.
    public int playerNr;
    // The number of the players vest.
    public int westNr;
    // The name of the player.
    public String name;
    // The team of the player.
    public String team;
    // The connection to the player.
    Socket conToPlayer;
    DataOutputStream outToPlayer;
    BufferedReader inFromPlayer;
}
