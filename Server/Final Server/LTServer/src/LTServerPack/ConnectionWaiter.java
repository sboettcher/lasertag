/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */

package LTServerPack;

import java.net.ServerSocket;
import java.net.Socket;
import javax.swing.SwingUtilities;
import java.io.IOException;

/**
 * A thread which waits asynchronous for an incoming TCP-connection.
 * 
 * @author Marc
 */
public class ConnectionWaiter extends Thread {
    
    /**
     * Contrustor. Sets some members.
     * 
     * @param lts - the main GUI.
     * @param iS - the global server socket.
     */
    public ConnectionWaiter(LTServer lts, ServerSocket iS) {
        inSocket = iS;
        ltts = lts;
        conToClient = null;
        running = true;
    }
    
    /**
     * A thread which runs until there is new socket request accepted and passed
     * over to the main GUI.
     */
    @Override
    public void run() {
        // Check if there is new socket and if the thread should still run.
        while(conToClient == null && running) {
            // Check for a new connection request.
            try {
                conToClient = inSocket.accept();
            } catch (IOException e) {
                System.err.println(e);
            }
        }
        // Schedule the handling of the new connection in the main GUI thread.
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                ltts.setSocket(conToClient);
            }
        });
    }
    
    // Public MEMBERS:
    // Indicator if the thread should run.
    public boolean running;
    
  // PRIVATE:
    
    // MEMBERS:
    // The global server socket.
    ServerSocket inSocket;
    // The main GUI.
    LTServer ltts;
    // The new socket connection from the client.
    Socket conToClient;
}
