/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */

package javatcpservertest;

import java.io.*;
import java.net.*;

/**
 * A simple little class which sets up a tcp server that returns all it's client inputs.
 */
class JavaTCPServerTest {
    public static void main(String argv[]) throws Exception {
        
        // Create a server socket.
        ServerSocket serverSocket = new ServerSocket(1234);
        String clientInput;
        while(true) { // The main loop.
            // Wait for an connection.
            Socket connectionSocket = serverSocket.accept();
            // Open an input and an output stream
            BufferedReader inFromClient = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
            DataOutputStream outToClient = new DataOutputStream(connectionSocket.getOutputStream());
            // Wait for an input.
            clientInput = inFromClient.readLine();
            // Print it.
            System.out.println("Received from client: " + clientInput);
            System.out.flush();
            // Return the input.
            outToClient.writeBytes(clientInput + '\n');
            outToClient.flush();
        }
    }
}