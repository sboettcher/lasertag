/*
 * Copyright: University of Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */

package LTServer;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * A class implementing a tcp server with little test GUI.
 * 
 * @author Marc
 */
public class LaserTagTestServer extends javax.swing.JFrame {

    /**
     * Constructor. Creates new form LaserTagTestServer and does some
     * intializing.
     */
    public LaserTagTestServer() {
        initComponents();
        // Initialize GUI.
        this.jButtonDisCon.setEnabled(false);
        this.jButtonCancel.setEnabled(false);
        Output = "";
        // Open the server socket.
        try{
            serverSocket = new ServerSocket(1234);
        } catch (IOException e) {
            System.err.println(e);
        }
    }
    
    /**
     * Handels new received data from the client.
     * 
     * @param data - the received data.
     */
    public void recData(String data) {
        // Show the new data in the GUI.
        Output = Output.concat(data);
        Output = Output.concat("\n");
        this.jTextAreaReceiveText.setText(Output);
    }
    
    /**
     * Handels a new received socket connection.
     * 
     * @param s - the new socket connection.
     */
    public void setSocket(Socket s) {
        // Save the socket.
        conToClient = s;
        // Create an input and an output stream to the client.
        BufferedReader inFromClient = null;
        try {
            outToClient = new DataOutputStream(conToClient.getOutputStream());
            inFromClient = new BufferedReader(new InputStreamReader(conToClient.getInputStream()));
        } catch (IOException e) {
            System.err.println(e);
        }
        // Start a read thread.
        if (inFromClient != null) {
            rThread = new LT_ServerReadThread(inFromClient, this);
            rThread.start();
        }
        // Update the GUI.
        this.jLabelConStatus.setText("Status: Connected");
        this.jButtonDisCon.setEnabled(true);
        this.jButtonCancel.setEnabled(false);
    }
    
    /**
     * Sends data to the client.
     * 
     * @param data - data which should be send.
     */
    private void sendData(String data) {
        // Concat a newline.
        String sendString = data.concat("\n");
        // Write the data to the client.
        if (this.outToClient != null) {
            try {
                this.outToClient.writeBytes(sendString);
                this.outToClient.flush();
            } catch (IOException e) {
                System.err.println(e);
            }
        }
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel1 = new javax.swing.JPanel();
        jButtonRecCon = new javax.swing.JButton();
        jLabelConStatus = new javax.swing.JLabel();
        jButtonDisCon = new javax.swing.JButton();
        jButtonCancel = new javax.swing.JButton();
        jPanel2 = new javax.swing.JPanel();
        jTextFieldSendText = new javax.swing.JTextField();
        jButtonSend = new javax.swing.JButton();
        jPanel3 = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        jTextAreaReceiveText = new javax.swing.JTextArea();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        jPanel1.setBorder(javax.swing.BorderFactory.createTitledBorder("Connection"));

        jButtonRecCon.setText("Receive New Connection");
        jButtonRecCon.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonRecConActionPerformed(evt);
            }
        });

        jLabelConStatus.setText("Satus: Disconnected");

        jButtonDisCon.setText("Disconnect");
        jButtonDisCon.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonDisConActionPerformed(evt);
            }
        });

        jButtonCancel.setText("Cancel");
        jButtonCancel.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonCancelActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jButtonRecCon)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jLabelConStatus)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 8, Short.MAX_VALUE)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jButtonCancel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jButtonDisCon, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButtonRecCon)
                    .addComponent(jLabelConStatus)
                    .addComponent(jButtonDisCon))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jButtonCancel)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel2.setBorder(javax.swing.BorderFactory.createTitledBorder("Send"));

        jTextFieldSendText.setText("Enter Text");

        jButtonSend.setText("Send");
        jButtonSend.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonSendActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jTextFieldSendText)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jButtonSend)
                .addContainerGap())
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jTextFieldSendText, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jButtonSend))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel3.setBorder(javax.swing.BorderFactory.createTitledBorder("Receive"));

        jTextAreaReceiveText.setColumns(20);
        jTextAreaReceiveText.setRows(5);
        jScrollPane1.setViewportView(jTextAreaReceiveText);

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1)
                .addContainerGap())
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jPanel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jPanel3, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jPanel2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jPanel3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    /**
     * Handler for a click on the receive new connection button. Prepares
     * everything to receive a new connection.
     * 
     * @param evt - the click event. 
     */
    private void jButtonRecConActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonRecConActionPerformed
        // Update the GUI.
        this.jLabelConStatus.setText("Status: Waiting...");
        this.jButtonCancel.setEnabled(true);
        // Start the waiting for a connection.
        cw = new ConnectionWaiter(this, serverSocket);
        cw.start();
    }//GEN-LAST:event_jButtonRecConActionPerformed

    /**
     * Handler for a click on the disconnect button. Closes and deletes the
     * conncetion.
     * 
     * @param evt - the click event. 
     */
    private void jButtonDisConActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonDisConActionPerformed
        // Stop the reading-thread.
        rThread.running = false;
        try {
            // Close the connection.
            outToClient.close();
            conToClient.close();
        } catch (IOException e) {
            System.err.println(e);
        }
        // Check if closing was successful and display it on the GUI.
        if (conToClient.isClosed()) {
            this.jLabelConStatus.setText("Status: Disconnected");
            this.jButtonDisCon.setEnabled(false);
        }
    }//GEN-LAST:event_jButtonDisConActionPerformed

    /**
     * Handler for a click on the send button. Send the currently entered
     * message.
     * 
     * @param evt - the click event. 
     */
    private void jButtonSendActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSendActionPerformed
        this.sendData(this.jTextFieldSendText.getText());
    }//GEN-LAST:event_jButtonSendActionPerformed

    /**
     * Handler for a click on the cancel button. Cancels the waiting for a
     * connection..
     * 
     * @param evt - the click event. 
     */
    private void jButtonCancelActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonCancelActionPerformed
        cw.running = false;
        this.jLabelConStatus.setText("Status: Disconnected");
        this.jButtonDisCon.setEnabled(false);
        this.jButtonCancel.setEnabled(false);
    }//GEN-LAST:event_jButtonCancelActionPerformed

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        /* Set the Nimbus look and feel */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
         */
        try {
            /*for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }*/
            javax.swing.UIManager.setLookAndFeel(
            javax.swing.UIManager.getSystemLookAndFeelClassName());
        } catch (ClassNotFoundException ex) {
            java.util.logging.Logger.getLogger(LaserTagTestServer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(LaserTagTestServer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(LaserTagTestServer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(LaserTagTestServer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>

        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                new LaserTagTestServer().setVisible(true);
            }
        });
    }

    // MEMBERS:
    ServerSocket serverSocket;  // The main server socket.
    Socket conToClient;  // The connection to the client.
    LT_ServerReadThread rThread;  // The read thread.
    DataOutputStream outToClient;  // The output stream to the client.
    String Output;  // The message to the client.
    ConnectionWaiter cw;  // The connection waiter.
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButtonCancel;
    private javax.swing.JButton jButtonDisCon;
    private javax.swing.JButton jButtonRecCon;
    private javax.swing.JButton jButtonSend;
    private javax.swing.JLabel jLabelConStatus;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JTextArea jTextAreaReceiveText;
    private javax.swing.JTextField jTextFieldSendText;
    // End of variables declaration//GEN-END:variables
}