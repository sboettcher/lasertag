/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package LTServerDemo;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;
import javax.swing.SwingUtilities;

/**
 *
 * @author Marc
 */
public class DemoGUI extends javax.swing.JFrame {

    /**
     * Creates new form DemoGUI
     */
    public DemoGUI() {
        initComponents();
        this.setLocationRelativeTo(null);
        try{
            serverSocket = new ServerSocket(1234);
        } catch (IOException e) {
            System.err.println(e);
        }
        playerCounter = 1; 
        this.jComboBoxSelectPlayer.addItem("No Player selected");
        Players = new ArrayList<Player>();
    }
    
    public void sendData(String data, int playerNr) {
        String dataToSend = data.concat("\n");
        if (Players.get(playerNr).outToPlayer != null) {
            try {
                this.Players.get(playerNr).outToPlayer.writeBytes(dataToSend);
                this.Players.get(playerNr).outToPlayer.flush();
            } catch (IOException e) {
                System.err.println(e);
            }
        }
    }
    
    public void newData(String data, int playerNr) {
        // String inp = data.substring(0, data.indexOf(":"));
        if(!data.equals("")) {
        Player hittenPlayer = Players.get(playerNr);
        Player shootPlayer = getPlyerByShootId(Integer.parseInt(data));
        sendData("" + shootPlayer.name, hittenPlayer.playerNr - 1);
        if (hittenPlayer.health - 25 < 0) {
            hittenPlayer.health = 0;
        } else {
            hittenPlayer.health = hittenPlayer.health - 25;
        }
        if (this.jComboBoxSelectPlayer.getSelectedIndex() == hittenPlayer.playerNr) {
            this.jLabelPlayerStats_HealthIn.setText("" + hittenPlayer.health + "/100");
            this.jProgressBarPlayerStats_Health.setValue(hittenPlayer.health);
        }
        String news = "";
        news = news.concat(shootPlayer.name);
        news = news.concat(" (");
        news = news.concat(shootPlayer.team);
        news = news.concat(") ");
        news = news.concat(" hit ");
        news = news.concat(hittenPlayer.name);
        news = news.concat(" (");
        news = news.concat(hittenPlayer.team);
        news = news.concat(") ");
        news = news.concat(" on the head!");
        this.jLabelNews.setText(news);
        if (newsTimer != null) {
            newsTimer.cancel();
        }
        newsTimer = new Timer();
        newsTimer.schedule(new TimerTask() {
            @Override
	    public void run(){
                SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run()
                    {
                        jLabelNews.setText("");
                    }
                });
	    }
        }, 5000);
        } else {
            System.out.println("Input-Data corrupt.");
        }
    }
    
    public Player getPlyerByShootId(int id) {
        for (int i = 0; i < Players.size(); i++) {
            if (Players.get(i).westNr == id) {
                return Players.get(i);
            }
        }
        return null;
    }
    
    public void setSocket(Socket s) {
        waitForCon.dispose();
        currentNewPlayer.conToPlayer = s;
        BufferedReader inFromClient = null;
        DataOutputStream outToClient = null;
        try {
            outToClient = new DataOutputStream(currentNewPlayer.conToPlayer.getOutputStream());
            inFromClient = new BufferedReader(new InputStreamReader(currentNewPlayer.conToPlayer.getInputStream()));
        } catch (IOException e) {
            System.err.println(e);
        }
        if (inFromClient != null) {
            currentNewPlayer.inFromPlayer = inFromClient;
        } else {
            // TODO Error.
        }
        if (outToClient != null) {
            currentNewPlayer.outToPlayer = outToClient;
        }
        currentNewPlayer.playerNr = playerCounter;
        this.jComboBoxSelectPlayer.addItem("Player " + playerCounter + " - " + currentNewPlayer.name);
        Players.add(currentNewPlayer);
        if (rThread != null) {
            rThread.running = false;
        }
        rThread = new LT_ServerReadThread(Players, this);
        rThread.start();
        this.jComboBoxSelectPlayer.setSelectedIndex(playerCounter);
        this.jLabelPlayerData_NameIn.setText(Players.get(playerCounter - 1).name);
        this.jLabelPlayerData_TeamIn.setText(Players.get(playerCounter - 1).team);
        this.jLabelPlayerData_WestNrIn.setText("" + Players.get(playerCounter - 1).westNr);
        this.jLabelPlayerStats_HealthIn.setText("" + Players.get(playerCounter - 1).health + "/100");
        this.jProgressBarPlayerStats_Health.setValue(Players.get(playerCounter - 1).health);
        playerCounter++;
    }
    
    public void conCanceled() {
        cw.running = false;
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        buttonGroupTeam = new javax.swing.ButtonGroup();
        jPanel1 = new javax.swing.JPanel();
        jLabelNews = new javax.swing.JLabel();
        jPanel2 = new javax.swing.JPanel();
        jComboBoxSelectPlayer = new javax.swing.JComboBox();
        jLabelSelectPlayer = new javax.swing.JLabel();
        jPanel4 = new javax.swing.JPanel();
        jLabelPlayerData_Name = new javax.swing.JLabel();
        jLabelPlayerData_NameIn = new javax.swing.JLabel();
        jLabelPlayerData_Team = new javax.swing.JLabel();
        jLabelPlayerData_TeamIn = new javax.swing.JLabel();
        jLabelPlayerData_WestNr = new javax.swing.JLabel();
        jLabelPlayerData_WestNrIn = new javax.swing.JLabel();
        jPanel5 = new javax.swing.JPanel();
        jLabelPlayerStats_Health = new javax.swing.JLabel();
        jProgressBarPlayerStats_Health = new javax.swing.JProgressBar();
        jLabelPlayerStats_HealthIn = new javax.swing.JLabel();
        jPanel3 = new javax.swing.JPanel();
        jLabelNewPlayer_Name = new javax.swing.JLabel();
        jTextFieldNewPlayer_Name = new javax.swing.JTextField();
        jLabelNewPlayer_WestNr = new javax.swing.JLabel();
        jTextFieldNewPlayer_WestNr = new javax.swing.JTextField();
        jLabelNewPlayer_Team = new javax.swing.JLabel();
        jRadioButtonTeamRed = new javax.swing.JRadioButton();
        jRadioButtonTeamBlue = new javax.swing.JRadioButton();
        jButtonAddPlayer = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("Laser Tag Server Demo");

        jPanel1.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "News", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 18))); // NOI18N

        jLabelNews.setFont(new java.awt.Font("Tahoma", 1, 18)); // NOI18N
        jLabelNews.setForeground(new java.awt.Color(255, 0, 0));
        jLabelNews.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabelNews, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jLabelNews, javax.swing.GroupLayout.PREFERRED_SIZE, 57, javax.swing.GroupLayout.PREFERRED_SIZE)
        );

        jPanel2.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Players", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 18))); // NOI18N

        jComboBoxSelectPlayer.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                jComboBoxSelectPlayerItemStateChanged(evt);
            }
        });

        jLabelSelectPlayer.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelSelectPlayer.setText("Select Player:");

        jPanel4.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Player-Data", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 14))); // NOI18N

        jLabelPlayerData_Name.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelPlayerData_Name.setText("Name:");

        jLabelPlayerData_NameIn.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelPlayerData_NameIn.setText("-");

        jLabelPlayerData_Team.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelPlayerData_Team.setText("Team:");

        jLabelPlayerData_TeamIn.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelPlayerData_TeamIn.setText("-");

        jLabelPlayerData_WestNr.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelPlayerData_WestNr.setText("West-Nr.:");

        jLabelPlayerData_WestNrIn.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelPlayerData_WestNrIn.setText("-");

        javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
        jPanel4.setLayout(jPanel4Layout);
        jPanel4Layout.setHorizontalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel4Layout.createSequentialGroup()
                        .addComponent(jLabelPlayerData_Team)
                        .addGap(18, 18, 18)
                        .addComponent(jLabelPlayerData_TeamIn)
                        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                    .addGroup(jPanel4Layout.createSequentialGroup()
                        .addComponent(jLabelPlayerData_Name)
                        .addGap(18, 18, 18)
                        .addComponent(jLabelPlayerData_NameIn)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(jLabelPlayerData_WestNr)
                        .addGap(18, 18, 18)
                        .addComponent(jLabelPlayerData_WestNrIn)
                        .addGap(20, 20, 20))))
        );
        jPanel4Layout.setVerticalGroup(
            jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel4Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabelPlayerData_Name)
                    .addComponent(jLabelPlayerData_NameIn)
                    .addComponent(jLabelPlayerData_WestNr)
                    .addComponent(jLabelPlayerData_WestNrIn))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabelPlayerData_Team)
                    .addComponent(jLabelPlayerData_TeamIn))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel5.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Player-Stats", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 14))); // NOI18N

        jLabelPlayerStats_Health.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelPlayerStats_Health.setText("Health:");

        jLabelPlayerStats_HealthIn.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelPlayerStats_HealthIn.setText("-");

        javax.swing.GroupLayout jPanel5Layout = new javax.swing.GroupLayout(jPanel5);
        jPanel5.setLayout(jPanel5Layout);
        jPanel5Layout.setHorizontalGroup(
            jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel5Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabelPlayerStats_Health)
                .addGap(18, 18, 18)
                .addComponent(jProgressBarPlayerStats_Health, javax.swing.GroupLayout.PREFERRED_SIZE, 234, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(jLabelPlayerStats_HealthIn)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        jPanel5Layout.setVerticalGroup(
            jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel5Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jLabelPlayerStats_HealthIn, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jLabelPlayerStats_Health, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jProgressBarPlayerStats_Health, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jPanel4, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(jPanel2Layout.createSequentialGroup()
                        .addComponent(jLabelSelectPlayer)
                        .addGap(18, 18, 18)
                        .addComponent(jComboBoxSelectPlayer, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(0, 0, Short.MAX_VALUE))
                    .addComponent(jPanel5, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabelSelectPlayer)
                    .addComponent(jComboBoxSelectPlayer, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel5, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jPanel3.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "New Player", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 14))); // NOI18N

        jLabelNewPlayer_Name.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelNewPlayer_Name.setText("Name:");

        jTextFieldNewPlayer_Name.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jTextFieldNewPlayer_Name.setText("Enter Name");

        jLabelNewPlayer_WestNr.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelNewPlayer_WestNr.setText("West-Nr.:");

        jTextFieldNewPlayer_WestNr.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jTextFieldNewPlayer_WestNr.setText("Enter Nr.");

        jLabelNewPlayer_Team.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jLabelNewPlayer_Team.setText("Team:");

        buttonGroupTeam.add(jRadioButtonTeamRed);
        jRadioButtonTeamRed.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jRadioButtonTeamRed.setSelected(true);
        jRadioButtonTeamRed.setText("Red");

        buttonGroupTeam.add(jRadioButtonTeamBlue);
        jRadioButtonTeamBlue.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jRadioButtonTeamBlue.setText("Blue");

        jButtonAddPlayer.setFont(new java.awt.Font("Tahoma", 0, 14)); // NOI18N
        jButtonAddPlayer.setText("Add Player");
        jButtonAddPlayer.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonAddPlayerActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel3Layout.createSequentialGroup()
                        .addComponent(jLabelNewPlayer_Name)
                        .addGap(18, 18, 18)
                        .addComponent(jTextFieldNewPlayer_Name, javax.swing.GroupLayout.PREFERRED_SIZE, 160, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(60, 60, 60)
                        .addComponent(jLabelNewPlayer_WestNr)
                        .addGap(18, 18, 18)
                        .addComponent(jTextFieldNewPlayer_WestNr, javax.swing.GroupLayout.DEFAULT_SIZE, 67, Short.MAX_VALUE))
                    .addGroup(jPanel3Layout.createSequentialGroup()
                        .addComponent(jLabelNewPlayer_Team)
                        .addGap(18, 18, 18)
                        .addComponent(jRadioButtonTeamRed)
                        .addGap(18, 18, 18)
                        .addComponent(jRadioButtonTeamBlue)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(jButtonAddPlayer)))
                .addContainerGap())
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel3Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabelNewPlayer_Name)
                    .addComponent(jTextFieldNewPlayer_Name, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabelNewPlayer_WestNr)
                    .addComponent(jTextFieldNewPlayer_WestNr, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 14, Short.MAX_VALUE)
                .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabelNewPlayer_Team)
                    .addComponent(jRadioButtonTeamRed)
                    .addComponent(jRadioButtonTeamBlue)
                    .addComponent(jButtonAddPlayer))
                .addContainerGap())
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
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jComboBoxSelectPlayerItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jComboBoxSelectPlayerItemStateChanged
        int itemNr = this.jComboBoxSelectPlayer.getSelectedIndex();
        if (itemNr == 0) {
            this.jLabelPlayerData_NameIn.setText("-");
            this.jLabelPlayerData_TeamIn.setText("-");
            this.jLabelPlayerData_WestNrIn.setText("-");
            this.jLabelPlayerStats_HealthIn.setText("-");
            this.jProgressBarPlayerStats_Health.setValue(0);
        } else {
            this.jLabelPlayerData_NameIn.setText(Players.get(itemNr - 1).name);
            this.jLabelPlayerData_TeamIn.setText(Players.get(itemNr - 1).team);
            this.jLabelPlayerData_WestNrIn.setText("" + Players.get(itemNr - 1).westNr);
            this.jLabelPlayerStats_HealthIn.setText("" + Players.get(itemNr - 1).health + "/100");
            this.jProgressBarPlayerStats_Health.setValue(Players.get(itemNr - 1).health);
        }
    }//GEN-LAST:event_jComboBoxSelectPlayerItemStateChanged

    private void jButtonAddPlayerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonAddPlayerActionPerformed
        currentNewPlayer = new Player();
        currentNewPlayer.health = 100;
        currentNewPlayer.westNr = Integer.parseInt(this.jTextFieldNewPlayer_WestNr.getText());
        currentNewPlayer.name = this.jTextFieldNewPlayer_Name.getText();
        if (this.jRadioButtonTeamRed.isSelected()) {
            currentNewPlayer.team = "Red";
        } else {
            currentNewPlayer.team = "Blue";
        }
        cw = new ConnectionWaiter(this, serverSocket);
        cw.start();
        waitForCon = new WaitForCon();
        waitForCon.setVisible(true);
    }//GEN-LAST:event_jButtonAddPlayerActionPerformed

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
            java.util.logging.Logger.getLogger(DemoGUI.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(DemoGUI.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(DemoGUI.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(DemoGUI.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>

        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                new DemoGUI().setVisible(true);
            }
        });
    }
    
    int playerCounter;
    Player currentNewPlayer;
    ConnectionWaiter cw;
    ServerSocket serverSocket;
    WaitForCon waitForCon;
    LT_ServerReadThread rThread;
    ArrayList<Player> Players;
    Timer newsTimer;
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.ButtonGroup buttonGroupTeam;
    private javax.swing.JButton jButtonAddPlayer;
    private javax.swing.JComboBox jComboBoxSelectPlayer;
    private javax.swing.JLabel jLabelNewPlayer_Name;
    private javax.swing.JLabel jLabelNewPlayer_Team;
    private javax.swing.JLabel jLabelNewPlayer_WestNr;
    private javax.swing.JLabel jLabelNews;
    private javax.swing.JLabel jLabelPlayerData_Name;
    private javax.swing.JLabel jLabelPlayerData_NameIn;
    private javax.swing.JLabel jLabelPlayerData_Team;
    private javax.swing.JLabel jLabelPlayerData_TeamIn;
    private javax.swing.JLabel jLabelPlayerData_WestNr;
    private javax.swing.JLabel jLabelPlayerData_WestNrIn;
    private javax.swing.JLabel jLabelPlayerStats_Health;
    private javax.swing.JLabel jLabelPlayerStats_HealthIn;
    private javax.swing.JLabel jLabelSelectPlayer;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JProgressBar jProgressBarPlayerStats_Health;
    private javax.swing.JRadioButton jRadioButtonTeamBlue;
    private javax.swing.JRadioButton jRadioButtonTeamRed;
    private javax.swing.JTextField jTextFieldNewPlayer_Name;
    private javax.swing.JTextField jTextFieldNewPlayer_WestNr;
    // End of variables declaration//GEN-END:variables
}
