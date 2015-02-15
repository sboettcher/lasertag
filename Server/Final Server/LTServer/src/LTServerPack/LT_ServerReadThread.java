/*
 * Copyright: University of Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */

package LTServerPack;

import java.io.IOException;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import java.util.ArrayList;

/**
 * A class implementing a reading-thread, that reads incoming data from a list
 * of given input streams and passes it over to the main GUI.
 */
public class LT_ServerReadThread extends Thread{
    
  // PUBLIC:
    
    /**
    * Constructor: Sets the list input-streams, the main GUI and initializes
    * some members.
    * 
    * @param p - list of players (including their input streams).
    * @param lts - the main GUI.
    */
    LT_ServerReadThread(ArrayList<Player> p, LTServer lts) {
        players = p;
        this.lts = lts;
        running = true;
        closed = false;
        delPlayer = false;
    }
    
    /**
    * Thread: Reads data form the input streams, and passes it over to the main
    * GUI.
    */
    @Override
    public void run(){
        // Check if a running-variable is set and the thread should run.
        while(this.running){
            // Loop over all players...
            for (int i = 0; i < players.size(); i++) {
                input = "";
                try {
                    // ...and check if there is new data available on their
                    // input stream.
                    if (this.running && players.get(i).inFromPlayer.ready()) {
                        // Read the data from the input-stream.
                        System.out.println("Read Input from client Nr. " + i);
                        // If the data doesn't come from an edison tagger, read
                        // it char by char since the newline at the end is maybe
                        // missing.
                        if (!players.get(i).edisonTagger) {
                            while(players.get(i).inFromPlayer.ready()) {
                                char c = (char) players.get(i).inFromPlayer.read();
                                input += c;
                            }
                        } else {
                            // If the data comes from an edison tagger, read a
                            // whole line.
                            this.input = this.players.get(i).inFromPlayer.readLine();
                        }
                    }
                } catch(IOException e){
                    // Give out an error-message if reading fails.
                    JOptionPane.showMessageDialog(null, "Couldn't read incoming"
                            + " data. Please check connection.", "Communication"
                            + " error.",
                            JOptionPane.ERROR_MESSAGE);
                    System.err.println(e);
                }

                // Check if the thread should still run.
                if (this.running && !input.equals("")) {
                    // Print out the incoming data for debug.
                    System.out.println("Input from client Nr. " + i + ": " + input);
                    // Copy the input data and the player number for the further
                    // processing.
                    final String newData = input;
                    final int playerNr = i;
                    // Update the GUI with the new values.
                    SwingUtilities.invokeLater(new Runnable() {
                        @Override
                        public void run()
                        {
                            lts.newData(newData, playerNr);
                        }
                    });
                }
            }
        }
        // Signalize that the thread closed and call a special method in the
        // main GUI if necessary.
        closed = true;
        if (delPlayer) {
            lts.threadFin();
        }
        System.out.println("ServerReadThread closed.");
    }
    
    // PUBLIC MEMBERS:
    // The running-variable.
    public boolean running;
    // States is the thread is closed,
    public boolean closed;
    // States if a player should be deleted -> Triggers a special method in the
    // main GUI after closing the thread.
    public boolean delPlayer;
    
  // PRIVATE:
    
    // MEMBERS
    private String input = "";  // Current input-data.
    LTServer lts;  // The main GUI.
    ArrayList<Player> players;  // The list of players.
}


