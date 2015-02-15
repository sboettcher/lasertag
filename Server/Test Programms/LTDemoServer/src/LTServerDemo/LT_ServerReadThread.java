/*
 * Copyright: University of Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */

package LTServerDemo;

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
     * @param mWi - the main GUI.
     */
    LT_ServerReadThread(ArrayList<Player> p, DemoGUI mWi) {
        players = p;
        ltts = mWi;
        running = true;
    }
    
    /**
    * Thread: Reads data form the input streams, and passes it over to the main
    * GUI.
    */
    @Override
    public void run(){
        // Check if a running-variable is set.
        while(this.running){
            // Loop over all players...
            for (int i = 0; i < players.size(); i++) {
                input = "";
                try {
                    // ...and check if there is new data available on their
                    // input stream.
                    if (this.running && players.get(i).inFromPlayer.ready()) {
                        // Read the data from the input-stream.
                        this.input = this.players.get(i).inFromPlayer.readLine();
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
                            ltts.newData(newData, playerNr);
                        }
                    });
                }
            }
        }
 
        System.out.println("ServerReadThread closed.");
    }
    
    // PUBLIC MEMBERS:
    // The running-variable.
    public boolean running;
    
  // PRIVATE:
    
    // MEMBERS:
    private String input = "";  // Current input-data.
    DemoGUI ltts;  // The main GUI.
    ArrayList<Player> players;  // The list of players.
}


