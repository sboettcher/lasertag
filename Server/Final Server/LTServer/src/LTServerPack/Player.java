/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */


package LTServerPack;

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
        playerNr = -1;
        westNr = -1;
        name = "missing";
        team = -1;
        teamColor = -1;
        health = -1;
        ammu = -1;
        points = -1;
        edisonTagger = true;
        conToPlayer = null;
        outToPlayer = null;
        inFromPlayer = null;
    }
    
    //MEMBERS:
    // The number of the player.
    public int playerNr;
    // The number of the players vest.
    public int westNr;
    // The name of the player.
    public String name;
    // The team of the player.
    public int team;
    // The teamcolor of the player.
    public int teamColor;
    // The current helath of the player.
    public int health;
    // The current ammunition of the player.
    public int ammu;
    // The current points of the player.
    public int points;
    // States if the tagger of the player has an edison controller in it.
    public boolean edisonTagger;
    // The connection to the player.
    Socket conToPlayer;
    DataOutputStream outToPlayer;
    BufferedReader inFromPlayer;
}