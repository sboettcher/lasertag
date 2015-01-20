/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package LTServerDemo;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.net.Socket;

/**
 *
 * @author Marc
 */
public class Player {
    
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
    
    public Player(int h, int w, String n, Socket s, int p, String t, BufferedReader i, DataOutputStream o) {
        health = h;
        westNr = w;
        name = n;
        playerNr = p;
        conToPlayer = s;
        team = t;
        outToPlayer = o;
        inFromPlayer = i;
    }
    
    public int health;
    public int playerNr;
    public int westNr;
    public String name;
    public String team;
    Socket conToPlayer;
    DataOutputStream outToPlayer;
    BufferedReader inFromPlayer;
}
