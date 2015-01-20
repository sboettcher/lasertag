/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package LTServerDemo;

import java.net.ServerSocket;
import java.net.Socket;
import javax.swing.SwingUtilities;
import java.io.IOException;

/**
 *
 * @author Marc
 */
public class ConnectionWaiter extends Thread {
    
    public ConnectionWaiter(DemoGUI mWi, ServerSocket iS) {
        inSocket = iS;
        ltts = mWi;
        conToClient = null;
        running = true;
    }
    
    @Override
    public void run() {
        while(conToClient == null && running) {
            try {
                conToClient = inSocket.accept();
            } catch (IOException e) {
                System.err.println(e);
            }
        }
        // Update the GUI with the new values.
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                ltts.setSocket(conToClient);
            }
        });
    }
    
    public boolean running;
    
  // PRIVATE:
    
    // MEMBERS:
    ServerSocket inSocket;
    DemoGUI ltts;
    Socket conToClient;
}
