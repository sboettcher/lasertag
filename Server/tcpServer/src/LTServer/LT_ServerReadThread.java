/*
 * Copyright: University of Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */
package LTServer;

import java.io.BufferedReader;
import java.io.IOException;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import java.net.Socket;

/**
 * A class implementing a reading-thread, that reads incoming data from a given
 * stream and decodes it and displays it.
 */
public class LT_ServerReadThread extends Thread{
    
  // PUBLIC:
    
    /**
    * Constructor: Sets the input-stream, the GUI and initializes some
    * membervariables.
    */
    LT_ServerReadThread(BufferedReader input, LaserTagTestServer mWi, Socket s) {  // TODO: *You have to adjust "MainWindow" to your main-GUI-class.*
        inFromServer = input;
        ltts = mWi;
        running = true;
        conToClient = s;
    }
    
    /**
    * Thread: Reads data form the input stream, decodes it and displays it on the
    * GUI.
    */
    @Override
    public void run(){
        // Check if a running-variable is set.
        while(this.running){
            try {
                // Check if new data on the input-stream are available.
                while(!this.inFromServer.ready()) {
                    // Check if the thread should close.
                    if(!this.running){
                        break;
                    }
                }
                // Check if the thread should still run.
                if (this.running) {
                    // Read the data from the input-stream.
                    this.input = this.inFromServer.readLine();
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
            if (this.running) {
                // Print out the incoming data for debug.
                System.out.println(input);
                // TODO: *You could do some decoding here. For example cut the input-string
                // and convert it to numbers.*
                               
                // Update the GUI with the new values.
                SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run()
                    {
                        // TODO: *Here you can call your functions in the GUI and transmit the
                        // new data (like below).*
                        ltts.recData(input);
                    }
                });
            }
        }
 
        System.out.println("ServerReadThread closed.");
    }
    
    // The running-variable.
    public boolean running;
    
  // PRIVATE:
    
    // Membervariables.
    private final BufferedReader inFromServer;  // Input-stream.
    private String input = "";  // Current input-data.
    LaserTagTestServer ltts;  
    Socket conToClient;
}


