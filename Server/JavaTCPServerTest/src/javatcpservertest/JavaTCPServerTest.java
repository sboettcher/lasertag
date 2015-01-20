/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package javatcpservertest;

import java.io.*;
import java.net.*;

class JavaTCPServerTest
{
   public static void main(String argv[]) throws Exception
      {
         String clientSentence;
         String capitalizedSentence;
         ServerSocket welcomeSocket = new ServerSocket(1234);

         while(true)
         {
            Socket connectionSocket = welcomeSocket.accept();
            BufferedReader inFromClient =
               new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
            DataOutputStream outToClient = new DataOutputStream(connectionSocket.getOutputStream());
            clientSentence = inFromClient.readLine();
            System.out.println("Received: " + clientSentence);
            System.out.flush();
            capitalizedSentence = clientSentence.toUpperCase() + '\n';
            // for (int i = 0; i < 10; i++) {
                outToClient.writeBytes(capitalizedSentence);
                outToClient.flush();
            // }
         }
      }
}