/*
 * Copyright: University of Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */

package LTServerPack;

import java.awt.Dimension;
import java.awt.Font;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import javax.swing.JLabel;
import java.util.ArrayList;
import javax.swing.border.TitledBorder;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;
import java.util.Timer;
import java.util.TimerTask;

/**
 * A class implmenting a server with GUI for a laser tag game.
 *
 * @author Marc
 */
public class LTServer extends javax.swing.JFrame {
    
  // PUBLIC:

    /**
     * Contructor. Creates new form LTServer and initializes it.
     */
    public LTServer() {
        initComponents();
        
        // Maximize the window to full-screen.
        this.setExtendedState(this.MAXIMIZED_BOTH);
        
        // Create new tab-dimensions.
        Dimension tabDim = new Dimension(); // Create dimension
        tabDim.height = 30; // Set height.
        tabDim.width = 135; // Set width.
        // Create new tab-font.
        Font f = new Font("Helvetica", 4, 18);
        
        // Modify the tabs:
        
        // Create a label for the overview panel.
        JLabel tabTitle_overviewPanel = new JLabel("Overview");
        tabTitle_overviewPanel.setPreferredSize(tabDim); // Set the dimensions.
        tabTitle_overviewPanel.setFont(f);  // Set the font.
        // Set the label as new tab-title.
        mainTabbedPane.setTabComponentAt(0, tabTitle_overviewPanel);
        
        // Create a label for the game settings panel.
        JLabel tabTitle_gameSettingsPanel = new JLabel("Game Settings");
        tabTitle_gameSettingsPanel.setPreferredSize(tabDim); // Set the dimensions.
        tabTitle_gameSettingsPanel.setFont(f);  // Set the font.
        // Set the label as new tab-title.
        mainTabbedPane.setTabComponentAt(1, tabTitle_gameSettingsPanel);
        
        // Create a label for the players panel.
        JLabel tabTitle_playersSettingsPanel = new JLabel("Add/Edit Players");
        tabTitle_playersSettingsPanel.setPreferredSize(tabDim); // Set the dimensions.
        tabTitle_playersSettingsPanel.setFont(f);  // Set the font.
        // Set the label as new tab-title.
        mainTabbedPane.setTabComponentAt(2, tabTitle_playersSettingsPanel);
        
        // Create a label for the game settings panel.
        JLabel tabTitle_rfidPanel = new JLabel("Add/Edit Tags");
        tabTitle_rfidPanel.setPreferredSize(tabDim); // Set the dimensions.
        tabTitle_rfidPanel.setFont(f);  // Set the font.
        // Set the label as new tab-title.
        mainTabbedPane.setTabComponentAt(3, tabTitle_rfidPanel);
        
        // Fill the team(-color)-selection-comboxes.
        this.firstTeamColorComboBox.removeAllItems();
        this.secondTeamColorComboBox.removeAllItems();
        for (int i = 0; i < colors.length; i++) {
            this.firstTeamColorComboBox.addItem(colors[i]);
            this.secondTeamColorComboBox.addItem(colors[i]);
        }
        
        // Initialize some members.
        playedGames = 0;
        currentPlayerNumber = 0;
        currentTagNumber = 0;
        maxHealth = 100;
        maxAmmu = 100;
        gameMode = 0;
        pointsPerTag = 50;
        ammuPerTag = 5;
        healthPerTag = 5;
        playTime = 12;
        firstTeamName = "Team1";
        firstTeamColor = 0; 
        secondTeamName = "Team2";
        secondTeamColor = 0;
        gamePaused = false;
        Players = new ArrayList<Player>();
        waitForSerial = true;
        tags = new ArrayList<Tag>();
        team1 = new ArrayList<Player>();
        team2 = new ArrayList<Player>();
        
        // Create a server socket on port 2000.
        try{
            serverSocket = new ServerSocket(2000);
        } catch (IOException e) {
            System.err.println(e);
        }
        
        // Scan for possible serial connections.
        serialHandler = new SerialHandler(this);
        String [] serialPorts = serialHandler.getSerialPorts();
        // If there are one or more possible serial connections, connect to the
        // first one.
        if (serialPorts.length > 0) {
            serialHandler.connect(serialPorts[0], 115200);
        }
        
        // Setup the GUI.
        this.tagAddButton.setEnabled(false);
        this.tagDeleteButton.setEnabled(false);
        this.tagCancelButton.setEnabled(false);
        this.playerDeleteButton.setEnabled(false);
        this.playerCancelButton.setEnabled(false);
        this.playedGamesInputLabel.setText("" + playedGames);
        this.playerAmmoProgressBar.setValue(100);
        this.playerHealthProgressBar.setValue(100);
        
        // Configure the Tables by settings the corresponding tabel models.
        fristTeamStatTable.setModel(new TableModelStats(team1));
        fristTeamStatTable.doLayout();
        secondTeamStatTable.setModel(new TableModelStats(team2));
        secondTeamStatTable.doLayout();
        tagListTable.setModel(new TableModelTags(tags));
        tagListTable.doLayout();
        playersListTable.setModel(new TableModelPlayer(Players));
        playersListTable.doLayout();
        // Add selection listener to the player and the tags tabel.
        tagListTable.getSelectionModel().addListSelectionListener(new ListSelectionListener(){
            @Override
            public void valueChanged(ListSelectionEvent event) {
                // do some actions here, for example
                // print first column value from selected row
                if (event.getValueIsAdjusting()) return;
                if (tagListTable.getSelectedRow() < 0) return;
                System.out.println("Selected Tag-No: " + tagListTable.getSelectedRow());
                editTag(tagListTable.getSelectedRow());
            }
        });
        playersListTable.getSelectionModel().addListSelectionListener(new ListSelectionListener(){
            @Override
            public void valueChanged(ListSelectionEvent event) {
                // do some actions here, for example
                // print first column value from selected row
                if (event.getValueIsAdjusting()) return;
                if (playersListTable.getSelectedRow() < 0) return;
                System.out.println(playersListTable.getSelectedRow());
                editPlayer(playersListTable.getSelectedRow());
            }
        });
        
        // Init top player list.
        String[] tp = {};
        topPlayerList.setListData(tp);
    }
    
    /**
     * Sorts all current players in decraesing order by their points and writes
     * the names with points in the top player list.
     */
    public void updateTopPlayer() {
        // Create the new top player list.
        String[] tp = new String[Players.size()];
        // Create a working copy of the curret players list.
        ArrayList<Player> tempPlayers = new ArrayList<Player>(Players);
        // Use a very simple algortihm to order the players by their points.
        for (int i = 0; i < Players.size(); i++) {
            int highscore = -1;
            int highScorePlayer = -1;
            for (int j = 0; j < tempPlayers.size(); j++) {
                if (tempPlayers.get(j).points > highscore) {
                    highScorePlayer = j;
                    highscore = tempPlayers.get(j).points;
                }
            }
            if (highScorePlayer > -1) {
                // Save the current top player in the list.
                tp[i] = "" + tempPlayers.get(highScorePlayer).name + " - "
                        + tempPlayers.get(highScorePlayer).points;
                tempPlayers.remove(highScorePlayer);
            }
        }
        // Show the list on the gui.
        topPlayerList.setListData(tp);
    }
    
    /**
     * Divides the current in player list in two team lists including all player
     * properties. Updates the GUI with this lists if necessary.
     * 
     * @param updateTeamListTable - stated if the GUI should be updated with
     * this lists.
     */
    public void updateTeamLists(boolean updateTeamListTable) {
        team1 = new ArrayList<Player>();
        team2 = new ArrayList<Player>();
        for (int i = 0; i < Players.size(); i++) {
            if (Players.get(i).team == 0)  {
                team1.add(Players.get(i));
            } else {
                team2.add(Players.get(i));
            }
        }
        if (updateTeamListTable) {
            fristTeamStatTable.setModel(new TableModelStats(team1));
            fristTeamStatTable.doLayout();
            secondTeamStatTable.setModel(new TableModelStats(team2));
            secondTeamStatTable.doLayout();
        }
    }
    
    /**
     * Updates the teams scores on the GUI.
     */
    public void updateTeamScore() {
        team1Score = 0;
        team2Score = 0;
        // Loop over the players and add the score of each player to the
        // corresponding team score.
        for (int i = 0; i < Players.size(); i++) {
            if (Players.get(i).team == 0)  {
                team1Score += Players.get(i).points;
            } else {
                team2Score += Players.get(i).points;
            }
        }
        // Display the scores on the GUI.
        this.firstTeamScoreLabel.setText("" + team1Score);
        this.secondTeamScoreLabel.setText("" + team2Score);
    }
    
    /**
     * Is called if connection attempt to server or the reading of new tag UID
     * is canceled.
     */
    public void conCanceled() {
        if (waitForSerial) {
            waitForSerial = false;
        } else {
            cw.running = false;
        }
    }
    
    /**
     * Is called if a new socket to player is established. Creates the new
     * player with all its properties, saves it and updates everthing.
     * @param s - the socket connection to the new player.
     */
    public void setSocket(Socket s) {
        // Close the waiting window.
        ww.dispose();
        // Create the new player.
        Player newPlayer = new Player();
        // Save the socket connection to him.
        newPlayer.conToPlayer = s;
        // Try to get an save the input and output from and to him.
        BufferedReader inFromClient = null;
        DataOutputStream outToClient = null;
        try {
            outToClient = new DataOutputStream(newPlayer.conToPlayer.getOutputStream());
            inFromClient = new BufferedReader(new InputStreamReader(newPlayer.conToPlayer.getInputStream()));
        } catch (IOException e) {
            System.err.println(e);
        }
        if (inFromClient != null) {
            newPlayer.inFromPlayer = inFromClient;
        }
        if (outToClient != null) {
            newPlayer.outToPlayer = outToClient;
        }
        // Save the given player properties.
        newPlayer.playerNr = currentPlayerNumber;
        newPlayer.health = currentHealth;
        newPlayer.westNr = currentVestNumber;
        newPlayer.name = currentPlayerName;
        newPlayer.team = currentTeam;
        newPlayer.ammu = currentAmmu;
        newPlayer.points = currentPoints;
        newPlayer.edisonTagger = currentTagger;
        if (currentTeam == 0) {
            newPlayer.teamColor = firstTeamColor;
        } else {
            newPlayer.teamColor = secondTeamColor;
        }
        // Add the new player.
        Players.add(newPlayer);
        // Start a new input read thread which includes the new player.
        if (readThread != null) {
            readThread.running = false;
        }
        readThread = new LT_ServerReadThread(Players, this);
        readThread.start();
        // Update the player list, the team list and the top player list.
        this.updateTeamLists(true);
        this.updateTeamScore();
        this.updateTopPlayer();
        playersListTable.setModel(new TableModelPlayer(Players));
        playersListTable.doLayout();
        // Increase the current player number and display it.
        currentPlayerNumber++;
        this.playerNumberTextField.setText("" + currentPlayerNumber);
        // Clear the selection in the player list tabel.
        playersListTable.clearSelection();
        
        // Send some initialization commands to the player.
        sendConInitCmd(currentPlayerNumber); // Send the vest-no.
        sendPlayerNameCmd(currentPlayerNumber); // Send the player name.
        sendGameModeCmd(currentPlayerNumber); // Send the game-mode.
        // Send the current ammunition decraese per tag.
        sendAmmuPerTagSetCmd(currentPlayerNumber, ammuPerTag);
        sendTeamSetCmd(currentPlayerNumber); // Send the team color.
        // Send the current heath decraese per tag.
        sendHealthPerTagSetCmd(currentPlayerNumber, healthPerTag);
        // Load the health to maximum.
        sendHealthSetCmd(currentPlayerNumber, maxHealth);
        // Load the ammunition to maximum.
        sendAmmuSetCmd(currentPlayerNumber, maxAmmu);
    }
    
    /**
     * This methos is called if there is new incomming data from a player.
     * 
     * @param newData - the incomming data.
     * @param playerNo - the number of the player which sends the data.
     */
    public void newData(String newData, int playerNo) {
        // Check if the received command is valid.
        int posBrack = newData.indexOf(">");
        if (newData.substring(0, 1).equals("<") && posBrack != -1) {
            // Extract the command, splitand print it.
            String[] cmd = newData.substring(1, posBrack).split(":");
            for (int i = 0; i < cmd.length; i++) {
                System.out.println(cmd[i]);
            }
            // Decode the command.
            String key = cmd[0];
            if (key.equals("in")) { // Info-command.
                // Print the info message.
                System.out.println(cmd[1]);
            } else if (key.equals("db")) {  // Debug message command.
                // Print the debug message.
                System.out.println(cmd[1]);
            } else if (key.equals("as")) {  // New ammu command.
                // Update the ammunition of the player and GUI.
                Players.get(playerNo).ammu = Integer.parseInt(cmd[1]);
                updateGUI();
            } else if (key.equals("ri")) {  // RFID request.
                // Check if the UID is not empty, parse and print it.
                if (!cmd[1].equals("")) {
                    byte[] uid = {  (byte) Integer.parseInt(cmd[1]
                                            .substring(0,3)), 
                                    (byte) Integer.parseInt(cmd[1]
                                            .substring(3,6)),
                                    (byte) Integer.parseInt(cmd[1]
                                            .substring(6,9)),
                                    (byte) Integer.parseInt(cmd[1]
                                            .substring(9,12))
                    };
                    System.out.println("UID from Client: " + getHexString(uid));
                    // Start a request for the UID.
                    rfidRequest(playerNo, uid);
                } else {
                    System.out.println("No RFID-Code");
                }
            } else if (key.equals("hi")) {  // Hit command.
                // Trigger a new hit.
                newHit(playerNo, Integer.parseInt(cmd[1]), 
                        Integer.parseInt(cmd[2]));
            } else {
                System.out.println("Unkown command.");
            }
        }
    }
    
    /**
     * Is called if a RFID tag is read by the serial reader. Saves the UID if
     * necessary.
     * 
     * @param rfidData - the UID.
     */
    public void newRFIDData(byte[] rfidData) {
        // Check if new data should be read and if it is valid.
        if (waitForSerial && rfidData.length >= 4) {
            // Close the waiting window if necessary.
            if (ww != null) {
                ww.dispose();
            }
            // Save the UID as bytes and hex string.
            currentUID = rfidData;
            currentUIDHexString = getHexString(rfidData);
            System.out.println(currentUIDHexString);
            // If the tag already exists in the database, load it, if not
            // prepare everything to save it.
            int tNo = checkUID(rfidData);
            if (tNo == -1) {
                this.tagUIDTextField.setText(currentUIDHexString);
            } else {
                editTag(tNo);
            }
            waitForSerial = false;
            this.tagAddButton.setEnabled(true);
        }
    }
    
    
  // PRIVATE:
    
    
    /**
     * Writes a given message to given player.
     * 
     * @param playerNo - player number.
     * @param msg - message to write.
     * @param withNL - indicates if there should be a newline at the end.
     */
    private void writeToPlayer(int playerNo, String msg, boolean withNL) {
        // Check if the player exists.
        if (Players.get(playerNo).outToPlayer != null) {
            // Try to write the message.
            try {
                if (withNL) {
                    Players.get(playerNo).outToPlayer.writeBytes(msg
                            .concat("\n"));
                } else {
                    Players.get(playerNo).outToPlayer.writeBytes(msg);
                }
                Players.get(playerNo).outToPlayer.flush();
            } catch (IOException e) {
                System.err.println(e);
            }
        }
    }
    
    /**
     * Writes given bytes to given player.
     * @param playerNo - player number.
     * @param b - bytes to write.
     */
    private void writeBytesToPlayer(int playerNo, byte[] b) {
        // Check if the player exists.
        if (Players.get(playerNo).outToPlayer != null) {
            // Try to write the bytes.
            try {
                for (int i = 0; i < b.length; i++) {
                    Players.get(playerNo).outToPlayer.writeByte(b[i]);
                }
                Players.get(playerNo).outToPlayer.flush();
            } catch (IOException e) {
                System.err.println(e);
            }
        }
    }
    
    /**
     * Transforms a given byte arry into an hey string.
     * 
     * @param b - the byte array.
     * @return - the hey string.
     */
    private String getHexString(byte[] b) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < b.length; i++){
            if (i > 0)
                sb.append(':');
            sb.append(Integer.toString((b[i] & 0xff) + 0x100, 16).substring(1));
        }
        return sb.toString();
    }
    
    /**
     * Returns the tag number for given UID.
     * 
     * @param uid - the UID.
     * @return - the tag number.
     */
    private int getTagNoByUID(byte[] uid) {
        for (int i = 0; i < tags.size(); i++) {
            if(tags.get(i).uid == uid) {
                return tags.get(i).tagNo;
            }
        }
        // If there is no tag with this number return -1.
        return -1;
    }
    
    /**
     * Returns the player number for a given vest number/id.
     * 
     * @param id - the id.
     * @return - the player number.
     */
    private int getPlayerNoById(int id) {
        for (int i = 0; i < Players.size(); i++) {
            if(Players.get(i).westNr == id) {
                return Players.get(i).playerNr;
            }
        }
        // If there is no player with this id return -1.
        return -1;
    }
    
    /**
     * Returns the tag object for a given UID.
     * 
     * @param uid - the UID.
     * @return - the tag object.
     */
    private Tag getTagByUID(byte[] uid) {
        for (int i = 0; i < tags.size(); i++) {
            if(tags.get(i).uid == uid) {
                return tags.get(i);
            }
        }
        // If there is no tag with this number return null.
        return null;
    }
    
    /**
     * Returns the player object for given vest number/id.
     * 
     * @param id - the id.
     * @return - the player object.
     */
    private Player getPlayerById(int id) {
        for (int i = 0; i < Players.size(); i++) {
            if(Players.get(i).westNr == id) {
                return Players.get(i);
            }
        }
        // If there is no player with this id return null.
        return null;
    }
    
    /**
     * Checks if a given UID already exists in tag list. If this is the case the
     * correspoanding tag number is returned.
     * 
     * @param uid - the uid whcih should be checked.
     * @return - the number of the corresponding tag or -1 if the UID doesn't
     * exist already.
     */
    private int checkUID(byte[] uid) {
        // Loop over all tags and compare the uids.
        for (int i = 0; i < tags.size(); i++) {
            if (uid[0] == tags.get(i).uid[0] && uid[1] == tags.get(i).uid[1]
                    && uid[2] == tags.get(i).uid[2] && uid[3] == tags.get(i).uid[3]) {
                // If the uid is equal, return the tag number.
                return tags.get(i).tagNo;
            }
        }
        // Return -1 if the uid couldn't be found in the list.
        return -1;
    }
    
    /**
     * Sets up the GUI so that an existing tag could be edited.
     * 
     * @param tagNo - the number of the tag which should be edited.
     */
    private void editTag(int tagNo) {
        // Change the title of the panel.
        TitledBorder tSP = (TitledBorder) this.tagSettingsPanel.getBorder();
        tSP.setTitle("Edit Tag");
        tagSettingsPanel.setBorder(tSP);
        repaint();
        // Activate the edit buttons.
        this.tagCancelButton.setEnabled(true);
        this.tagDeleteButton.setEnabled(true);
        this.tagAddButton.setEnabled(true);
        this.tagAddButton.setText(" Save ");
        // Load the properties of the tag.
        this.tagNumberTextField.setText("" + tagNo);
        this.tagUIDTextField.setText(tags.get(tagNo).uidHexString);
        this.tagNameTextField.setText(tags.get(tagNo).name);
        this.tagReloadAmmoCheckBox.setSelected(tags.get(tagNo).reloadAmmu);
        this.tagReloadHealthCheckBox.setSelected(tags.get(tagNo).reloadHealth);
        this.tagSetAmmoCheckBox.setSelected(tags.get(tagNo).ammuTo);
        this.tagSetHealthCheckBox.setSelected(tags.get(tagNo).healthTo);
        this.tagAddPointsCheckBox.setSelected(tags.get(tagNo).pointsTo);
        this.tagSetAmmoTextField.setText("" + tags.get(tagNo).ammuToValue);
        this.tagSetHelathTextField.setText("" + tags.get(tagNo).healthToValue);
        this.tagAddPointsTextField.setText("" + tags.get(tagNo).pointsToValue);
    }
    
    /**
     * Removes a given tag from the tag list and adjusts the tag numbers of the
     * remaining ones.
     * 
     * @param tagNo - the number of the tag which should be removed.
     */
    private void removeTag(int tagNo) {
        tags.remove(tagNo);
        for (int i = 0; i < tags.size(); i++) {
            tags.get(i).tagNo = i;
        }
    }
    
    /**
     * Sets up the GUI so that an existing player could be edited.
     * 
     * @param playerNo 
     */
    private void editPlayer(int playerNo) {
        // Change the title of the panel.
        TitledBorder pSP = (TitledBorder) this.playerSettingsPanel.getBorder();
        pSP.setTitle("Edit Player");
        tagSettingsPanel.setBorder(pSP);
        repaint();
         // Activate the edit buttons.
        this.playerCancelButton.setEnabled(true);
        this.playerDeleteButton.setEnabled(true);
        this.playerAddButton.setEnabled(true);
        this.playerAddButton.setText("  Save  ");
        // Load the properties of the player.
        this.playerNumberTextField.setText(""  + playerNo);
        this.vestNumberTextField.setText("" + Players.get(playerNo).westNr);
        this.playerNameTextField.setText(Players.get(playerNo).name);
        if (Players.get(playerNo).edisonTagger) {
            this.edisonRadioButton.setSelected(true);
        } else {
            this.arduinoRadioButton.setSelected(true);
        }
        this.playerPointsTextField.setText("" + Players.get(playerNo).points);
        // Health:
        int health = Players.get(playerNo).health;
        this.playerHelathTextField.setText("" + health);
        if (health > maxHealth) health = maxHealth;
        int healthBarValue = health / (maxHealth / 100);
        this.playerHealthProgressBar.setValue(healthBarValue);
        // Ammunition:
        int ammu = Players.get(playerNo).ammu;
        this.playerAmmoTextField.setText("" + ammu);
        if (ammu > maxAmmu) ammu = maxAmmu;
        int ammuBarValue = ammu / (maxAmmu / 100);
        this.playerAmmoProgressBar.setValue(ammuBarValue);
    }
    
    /**
     * Removes a given player from the player list and adjusts the tag numbers
     * of the remaining ones.
     * 
     * @param PlayerNo - the number of the player should be removed.
     */
    private void removePlayer(int PlayerNo) {
        Players.remove(PlayerNo);
        for (int i = 0; i < Players.size(); i++) {
            Players.get(i).playerNr = i;
        }
    }
    
    /**
     * Updates the team lists, the team scores and the top players list on the
     * GUI.
     */
    private void updateGUI() {
        this.updateTeamLists(true);
        this.updateTeamScore();
        this.updateTopPlayer();
    }
    
    /**
     * Updates the game timer on the GUI.
     */
    public void updateTimerOnGui() {
        // Calculate the minute and second value out of the second-counter.
	int sec = timeInSec % 60;
	int min = timeInSec / 60;
	// If necessary add leading zeros to the well that it looks nice.
	if (sec < 10 && min < 10) {
            remainingTimeInputLabel.setText("0" + min + ":0" + sec);
	} else if (sec < 10) {
            remainingTimeInputLabel.setText("" + min + ":0" + sec);
	} else if (min < 10) {
            remainingTimeInputLabel.setText("0" + min + ":" + sec);
	} else {
            remainingTimeInputLabel.setText("" + min + ":" + sec);
        }
        checkTime();
    }
    
    /**
     * Checks if the timer of a game is over.
     */
    private void checkTime() {
        if (timeInSec >= playTime) {
            gameTimer.cancel(); // Stop the timer.
            sendStopGameCmd(); // Send stop to all players.
            playedGames++; // Increase the played games counter.
            // Adjust the GUI.
            this.playedGamesInputLabel.setText("" + playedGames);
            this.gameStatusInputLabel.setText("Game stopped!");
        }
    }
    
    /**
     * This method is called if read thread has stopped so a player could be
     * deleted.
     */
    public void threadFin() {
        // Delete the player.
        System.out.println("Delete Player now");
        removePlayer(delPlayer);
        playersListTable.setModel(new TableModelPlayer(Players));
        playersListTable.doLayout();
        // Restart the read thread.
        if (Players.size() > 0) {
            readThread = new LT_ServerReadThread(Players, this);
            readThread.run();
        }
        currentPlayerNumber--;
        // Adjust the title of the panel for adding new players.
        TitledBorder pSP = (TitledBorder) this.playerSettingsPanel.getBorder();
        pSP.setTitle("New Player");
        tagSettingsPanel.setBorder(pSP);
        repaint();
        // Deactivate the editing buttons.
        this.playerCancelButton.setEnabled(false);
        this.playerDeleteButton.setEnabled(false);
        this.playerAddButton.setEnabled(true);
        this.playerAddButton.setText("Add Player");
        // Clear the property-flieds.
        this.playerNumberTextField.setText(""  + currentPlayerNumber);
        this.vestNumberTextField.setText("");
        this.playerNameTextField.setText("");
        this.edisonRadioButton.setSelected(true);
        this.playerPointsTextField.setText("0");
        // Health:
        int health = 100;
        this.playerHelathTextField.setText("" + health);
        if (health > maxHealth) health = maxHealth;
        int healthBarValue = health / (maxHealth / 100);
        this.playerHealthProgressBar.setValue(healthBarValue);
        // Ammunition:
        int ammu = 100;
        this.playerAmmoTextField.setText("" + ammu);
        if (ammu > maxAmmu) ammu = maxAmmu;
        int ammuBarValue = ammu / (maxAmmu / 100);
    }
    
    
    // COMMAND-METHODS:
    
    
    /**
     * Handles a new hit.
     * 
     * @param hiddenPlayerNo - the number of the hidden player.
     * @param hitPlayerID - the number of the player who hit.
     * @param hitPlace - the placde of the hit (not used until now).
     */
    private void newHit(int hiddenPlayerNo, int hitPlayerID, int hitPlace) {
        // Get the hidden- and the hit-player.
        Player hitPlayer = getPlayerById(hitPlayerID);
        Player hiddenPlayer = Players.get(hiddenPlayerNo);
        if (hitPlayer != null) {
            // Inform the players about the hit.
            sendTaggerNameCmd(hiddenPlayerNo, hitPlayer.name);
            sendTaggedNameCmd(hitPlayer.playerNr, hiddenPlayer.name);
            // Increase the points of the hit-player and send them to him.
            hitPlayer.points += pointsPerTag;
            sendPointsSetCmd(hitPlayer.playerNr, hitPlayer.points);
            // Increase the health of the hidden player and send it to him.
            if (hiddenPlayer.health - healthPerTag >= 0) {
                hiddenPlayer.health -= healthPerTag;
            } else {
                hiddenPlayer.health = 0;
            }
            sendHealthSetCmd(hiddenPlayerNo, hiddenPlayer.health);
            // Update the GUI.
            updateGUI();
        } else {
            System.out.println("Hitplayer unknown!");
        }
    }
    
    /**
     * Handles a new RFID/tag request from a player.
     * 
     * @param PlayerNo - the player number.
     * @param uid - the requested UID.
     */
    private void rfidRequest(int PlayerNo, byte[] uid) {
        // Get the tag number for the requested UID.
        int tagNo = checkUID(uid);
        // If the tag exists in the tag list handle it.
        if (tagNo != -1) {
            // Get the tag.
            Tag tag = tags.get(tagNo);
            // Reload the health if that is activated for the tag.
            if (tag.reloadHealth) {
                sendHealthSetCmd(PlayerNo, maxHealth);
                Players.get(PlayerNo).health = maxHealth;
            }
            // Reload the ammunition if that is activated for the tag.
            if (tag.reloadAmmu) {
                for (int i = 0; i < 3; i++) {
                    sendAmmuSetCmd(PlayerNo, maxAmmu);
                }
                Players.get(PlayerNo).ammu = maxAmmu;
            }
            // Set the ammunition to given value if that is activated for the tag.
            if (tag.ammuTo) {
                for (int i = 0; i < 3; i++) {
                    sendAmmuSetCmd(PlayerNo, tag.ammuToValue);
                }
                Players.get(PlayerNo).ammu = tag.ammuToValue;
            }
            // Set the health to given value if that is activated for the tag.
            if (tag.healthTo) {
                for (int i = 0; i < 3; i++) {
                    sendHealthSetCmd(PlayerNo, tag.healthToValue);
                }
                Players.get(PlayerNo).health = tag.healthToValue;
            }
            // Set the points to given value if that is activated for the tag.
            if (tag.pointsTo) {
                for (int i = 0; i < 3; i++) {
                    sendPointsSetCmd(PlayerNo, tag.pointsToValue);
                }
                Players.get(PlayerNo).points = tag.pointsToValue;
            }
            // Update the GUI.
            updateGUI();
        } else {
            // If the tag doesn't exists in the tag list signalize that.
            System.out.println("Tag unknown!");
            sendInfoCmd(PlayerNo, "Tag unknown!");
        }
    }
    
    /**
     * Sends a start game command to all players. (At the moment only for
     * arduino tagger.)
     */
    private void sendStartGameCmd() {
        for (int i = 0; i < Players.size(); i++) {
            writeToPlayer(i, "<!gs>", true);
        }
    }
    
    /**
     * Sends a stop game command to all players. (At the moment only for
     * arduino tagger.)
     */
    private void sendStopGameCmd() {
        for (int i = 0; i < Players.size(); i++) {
            writeToPlayer(i, "<!gt>", true);
        }
    }
    
    /**
     * Sends a pause game command to all players. (At the moment only for
     * arduino tagger.)
     */
    private void sendPauseGameCmd() {
        for (int i = 0; i < Players.size(); i++) {
            writeToPlayer(i, "<!gp>", true);
        }
    }
    
    /**
     * Send the initialization command to a player. This command includes, if
     * there is a vest the tagger should connect to or not. 
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     */
    private void sendConInitCmd(int playerNo) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Check if 5 or 6 are selected as vest number -> Placeholders for
            // "no vest".
            if(Players.get(playerNo).westNr == 5 || Players.get(playerNo)
                    .westNr == 6) {
                // Build the string which say that there is no west and send it.
                String cmd = "<ci:0:" + Players.get(playerNo).westNr + ">";
                writeToPlayer(playerNo, cmd, true);
            } else {
                // If there is another vest number given, build a string which
                // says that there is a vest, include the vest number and send
                // it.
                String cmd = "<ci:1:" + Players.get(playerNo).westNr + ">";
                writeToPlayer(playerNo, cmd, true);
            }
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Send the start of the command.
            writeToPlayer(playerNo, "<!ci:", false);
            // Check if 5 or 6 are selected as vest number -> Placeholders for
            // "no vest".
            if(Players.get(playerNo).westNr == 5 || Players.get(playerNo)
                    .westNr == 6) {
                // Send 0 as byte -> means no vest.
                byte[] b = {(byte) 0};
                writeBytesToPlayer(playerNo, b);
            } else {
                // If there is another vest number, send 1 as byte -> means with
                // vest.
                byte[] b = {(byte) 1};
                writeBytesToPlayer(playerNo, b);
            }
            // Send the vest number and the end of the command.
            writeToPlayer(playerNo, ":", false);
            byte[] c = {(byte) Players.get(playerNo).westNr};
            writeBytesToPlayer(playerNo, c);
            writeToPlayer(playerNo, ">", true);
        }
    }
    
    /**
     * Sends the player name to a given player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     */
    private void sendPlayerNameCmd(int playerNo) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<np:" + Players.get(playerNo).name + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!".
            // Build the command string and send it to the player.
            String cmd = "<!np:" + Players.get(playerNo).name + ">";
            writeToPlayer(playerNo, cmd, true);
        }
    }
    
    /**
     * Sends the current game mode to given player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     */
    private void sendGameModeCmd(int playerNo) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<gm:" + gameMode + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Send the start of the command.
            writeToPlayer(playerNo, "<!gm:", false);
            // Send the game mode as byte.
            byte[] b = {(byte) gameMode};
            writeBytesToPlayer(playerNo, b);
            // Send the end of the command.
            writeToPlayer(playerNo, ">", true);
        }
    }
    
    /**
     * Sends specials to given player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     */
    private void sendGameSpecialCmd(int playerNo) {
        // Not needed at the moment -> Not implemented.
    }
    
    /**
     * Sends an arbitrary info message to  a given player
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param info - the info message.
     */    
    private void sendInfoCmd(int playerNo, String info) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<in:" + info + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Build the command string and send it to the player.
            String cmd = "<!in:" + info + ">";
            writeToPlayer(playerNo, cmd, true);
        }
    }
    
    /**
     * Send the current team color to given player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     */
    private void sendTeamSetCmd(int playerNo) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<ts:" + colors[Players.get(playerNo).teamColor] + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Send the start of the command.
            writeToPlayer(playerNo, "<!ts:", false);
            // Send the team color as byte.
            int color =  Players.get(playerNo).teamColor;
            byte[] b = colorsRGB[color];
            writeBytesToPlayer(playerNo, b);
            // Send the end of the command.
            writeToPlayer(playerNo, ">", true);
        }
    }
    
    /**
     * Sends a command which sets the health of given player to given value.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param health - the health value which should be set.
     */
    private void sendHealthSetCmd(int playerNo, int health) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<hs:" + health + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Send the start of the command.
            writeToPlayer(playerNo, "<!hs:", false);
            // Send the health value as byte.
            byte[] b = {(byte) health};
            writeBytesToPlayer(playerNo, b);
            // Send the end of the command.
            writeToPlayer(playerNo, ">", true);
        }
    }
    
    /**
     * Sends a command which sets the points of given player to given value.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param points - the points value which should be set.
     */
    private void sendPointsSetCmd(int playerNo, int points) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<ps:" + points + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Send the start of the command.
            writeToPlayer(playerNo, "<!ps:", false);
            // Send the points value as byte.
            byte[] b = {(byte) points};
            writeBytesToPlayer(playerNo, b);
            // Send the end of the command.
            writeToPlayer(playerNo, ">", true);
        }
    }
    
    /**
     * Sends a command which sets the ammunition of given player to given value.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param ammu - the ammunition value which should be set.
     */
    private void sendAmmuSetCmd(int playerNo, int ammu) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<as:" + ammu + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Send the start of the command.
            writeToPlayer(playerNo, "<!as:", false);
            // Send the ammunition value as byte.
            byte[] b = {(byte) ammu};
            writeBytesToPlayer(playerNo, b);
            // Send the end of the command.
            writeToPlayer(playerNo, ">", true);
        }
    }
    
    /**
     * Sends a command which sets the ammunition per tag of given player to
     * given value.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param ammuPerTag - the ammunition per tag value which should be set.
     */
    private void sendAmmuPerTagSetCmd(int playerNo, int ammuPerTag) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<ap:" + ammuPerTag + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Send the start of the command.
            writeToPlayer(playerNo, "<!ap:", false);
            // Send the ammunition per tag value as byte.
            byte[] b = {(byte) ammuPerTag};
            writeBytesToPlayer(playerNo, b);
            // Send the end of the command.
            writeToPlayer(playerNo, ">", true);
        }
    }
    
    /**
     * Sends a command which sets the health per tag of given player to
     * given value.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param healthPerTag - the health per tag value which should be set.
     */
    private void sendHealthPerTagSetCmd(int playerNo, int healthPerTag) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<hd:" + healthPerTag + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Send the start of the command.
            writeToPlayer(playerNo, "<!hd:", false);
            // Send the health per tag value as byte.
            byte[] b = {(byte) healthPerTag};
            writeBytesToPlayer(playerNo, b);
            // Send the end of the command.
            writeToPlayer(playerNo, ">", true);
        }
    }
    
    /**
     * Sends a command which triggers a given ammunition increase/decrease over
     * time to given player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param increase - state if there should be an increase. If flase ->
     * decrease.
     * @param incDecRate - the increase/decrease rate.
     */
    private void sendIncDecAmmuCmd(int playerNo, boolean increase, int incDecRate) {
        
        // Not needed at the moment -> Not really implemented.
        
        /*String cmd;
        if (increase) {
            cmd = "<ra:1" + incDecRate + ">";
        } else {
            cmd = "<ra:0" + incDecRate + ">"; 
        }
        writeToPlayer(playerNo, cmd, true);*/
    }
    
    /**
     * Sends a command which triggers a given health increase/decrease over
     * time to given player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param increase - state if there should be an increase. If flase ->
     * decrease.
     * @param incDecRate - the increase/decrease rate.
     */
    private void sendIncDecHealthCmd(int playerNo, boolean increase, int incDecRate) {
        
        // Not needed at the moment -> Not really implemented.
        
        /*String cmd;
        if (increase) {
            cmd = "<rh:1" + incDecRate + ">";
        } else {
            cmd = "<rh:0" + incDecRate + ">"; 
        }
        writeToPlayer(playerNo, cmd, true);*/
    }
    
    /**
     * Sends a command which triggers a given points increase/decrease over
     * time to given player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param increase - state if there should be an increase. If flase ->
     * decrease.
     * @param incDecRate - the increase/decrease rate.
     */
    private void sendIncDecPointsCmd(int playerNo, boolean increase, int incDecRate) {
        
        // Not needed at the moment -> Not really implemented.
        
        /*String cmd;
        if (increase) {
            cmd = "<tp:1" + incDecRate + ">";
        } else {
            cmd = "<tp:0" + incDecRate + ">"; 
        }
        writeToPlayer(playerNo, cmd, true);*/
    }
    
    /**
     * Send to player which was tagged the name of player which tagged him.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param tagger - the name of the player which tagged.
     */
    private void sendTaggerNameCmd(int playerNo, String tagger) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<hp:" + tagger + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Build the command string and send it to the player.
            String cmd = "<!hp:" + tagger + ">";
            writeToPlayer(playerNo, cmd, true);
        }
    }
    
    /**
     * Send to player which tagged another player the name of the tagged player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param tagged - name of the tagged player.
     */
    private void sendTaggedNameCmd(int playerNo, String tagged) {
        // Check if the given player has an edison tagger -> Normal command 
        // complete as string.
        if (Players.get(playerNo).edisonTagger) {
            // Build the command string and send it to the player.
            String cmd = "<hv:" + tagged + ">";
            writeToPlayer(playerNo, cmd, true);
        } else {
            // If the player has an arduino tagger -> Special command with an
            // extra "!" and numbers as bytes.
            // Build the command string and send it to the player.
            String cmd = "<!hv:" + tagged + ">";
            writeToPlayer(playerNo, cmd, true);
        }
    }
    
    /**
     * Send an activation/deactivation command to a given player.
     * 
     * @param playerNo - the number of the player which should receive the
     * command.
     * @param active - states if the player should be active or not. 
     */
    private void sendDeActivateTaggerCmd(int playerNo, boolean active) {
        
        // Not needed at the moment -> Not really implemented.
        
        /*String cmd;
        if (active) {
            cmd = "<at:1>";
        } else {
            cmd = "<at:0>";
        }
        writeToPlayer(playerNo, cmd, true);*/
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        taggerTypButtonGroup = new javax.swing.ButtonGroup();
        mainTabbedPane = new javax.swing.JTabbedPane();
        overviewPanel = new javax.swing.JPanel();
        statisticsPanel = new javax.swing.JPanel();
        firstTeamStatPanel = new javax.swing.JPanel();
        fristTeamStatScrollPane = new javax.swing.JScrollPane();
        fristTeamStatTable = new javax.swing.JTable();
        firstTeamScorePanel = new javax.swing.JPanel();
        firstTeamScoreLabel = new javax.swing.JLabel();
        secondTeamStatPanel = new javax.swing.JPanel();
        secondTeamStatScrollPane = new javax.swing.JScrollPane();
        secondTeamStatTable = new javax.swing.JTable();
        secondTeamScorePanel = new javax.swing.JPanel();
        secondTeamScoreLabel = new javax.swing.JLabel();
        topPlayerPanel = new javax.swing.JPanel();
        topPlayerScrollPane = new javax.swing.JScrollPane();
        topPlayerList = new javax.swing.JList();
        GameStatPanel = new javax.swing.JPanel();
        gameModeTitelLabel = new javax.swing.JLabel();
        gameModeInputLabel = new javax.swing.JLabel();
        playedGamesTitelLabel = new javax.swing.JLabel();
        playedGamesInputLabel = new javax.swing.JLabel();
        remainingTimeTitleLabel = new javax.swing.JLabel();
        remainingTimeInputLabel = new javax.swing.JLabel();
        gameStatusLabel = new javax.swing.JLabel();
        gameStatusInputLabel = new javax.swing.JLabel();
        gameControlPanel = new javax.swing.JPanel();
        startGameButton = new javax.swing.JButton();
        stopGameButton = new javax.swing.JButton();
        pauseGameButton = new javax.swing.JButton();
        gameSettingsPanel = new javax.swing.JPanel();
        gameModePanel = new javax.swing.JPanel();
        gameModeLabel = new javax.swing.JLabel();
        gameModeComboBox = new javax.swing.JComboBox();
        setPlayTimePanel = new javax.swing.JPanel();
        setPlayTimeLabel = new javax.swing.JLabel();
        setPlayTimeTextField = new javax.swing.JTextField();
        playTimeMinLabel = new javax.swing.JLabel();
        setGameSettingsPanel = new javax.swing.JPanel();
        maxAmmoLabel = new javax.swing.JLabel();
        maxHealthLabel = new javax.swing.JLabel();
        pointsPerTagLabel = new javax.swing.JLabel();
        ammoPerTagLabel = new javax.swing.JLabel();
        healthPerTagLabel = new javax.swing.JLabel();
        maxAmmoTextField = new javax.swing.JTextField();
        maxHealthTextField = new javax.swing.JTextField();
        pointsPerTagTextField = new javax.swing.JTextField();
        ammoPerTagTextField = new javax.swing.JTextField();
        healthPerTagTextField = new javax.swing.JTextField();
        teamSettingsPanel = new javax.swing.JPanel();
        firstTeamSettingsPanel = new javax.swing.JPanel();
        firstTeamColorLabel = new javax.swing.JLabel();
        firstTeamColorComboBox = new javax.swing.JComboBox();
        firstTeamNameLabel = new javax.swing.JLabel();
        firstTeamNameTextField = new javax.swing.JTextField();
        secondTeamSettingsPanel = new javax.swing.JPanel();
        secondTeamColorComboBox = new javax.swing.JComboBox();
        secondTeamColorLabel = new javax.swing.JLabel();
        secondTeamNameLabel = new javax.swing.JLabel();
        secondTeamNameTextField = new javax.swing.JTextField();
        saveGameSettingsButton = new javax.swing.JButton();
        playerPanel = new javax.swing.JPanel();
        playerSettingsPanel = new javax.swing.JPanel();
        playerGeneralPanel = new javax.swing.JPanel();
        playerNumberLabel = new javax.swing.JLabel();
        playerNumberTextField = new javax.swing.JTextField();
        playerNameLabel = new javax.swing.JLabel();
        playerNameTextField = new javax.swing.JTextField();
        vestNumberLabel = new javax.swing.JLabel();
        vestNumberTextField = new javax.swing.JTextField();
        playerTeamLabel = new javax.swing.JLabel();
        playerTeamComboBox = new javax.swing.JComboBox();
        edisonRadioButton = new javax.swing.JRadioButton();
        arduinoRadioButton = new javax.swing.JRadioButton();
        playerHealthPanel = new javax.swing.JPanel();
        playerHelathLabel = new javax.swing.JLabel();
        playerHelathTextField = new javax.swing.JTextField();
        playerHealthProgressBar = new javax.swing.JProgressBar();
        playerAmmoPanel = new javax.swing.JPanel();
        playerAmmoLabel = new javax.swing.JLabel();
        playerAmmoTextField = new javax.swing.JTextField();
        playerAmmoProgressBar = new javax.swing.JProgressBar();
        playerPointsPanel = new javax.swing.JPanel();
        playerPointsLabel = new javax.swing.JLabel();
        playerPointsTextField = new javax.swing.JTextField();
        playerAddButton = new javax.swing.JButton();
        playerCancelButton = new javax.swing.JButton();
        playerDeleteButton = new javax.swing.JButton();
        playerListPanel = new javax.swing.JPanel();
        playersListScrollPane = new javax.swing.JScrollPane();
        playersListTable = new javax.swing.JTable();
        rfidPanel = new javax.swing.JPanel();
        tagSettingsPanel = new javax.swing.JPanel();
        tagGeneralPanel = new javax.swing.JPanel();
        tagNumberLabel = new javax.swing.JLabel();
        tagNumberTextField = new javax.swing.JTextField();
        tagUIDLabel = new javax.swing.JLabel();
        tagUIDTextField = new javax.swing.JTextField();
        tagNameLabel = new javax.swing.JLabel();
        tagNameTextField = new javax.swing.JTextField();
        newTagButton = new javax.swing.JButton();
        tagFunctionPanel = new javax.swing.JPanel();
        tagReloadAmmoCheckBox = new javax.swing.JCheckBox();
        tagReloadHealthCheckBox = new javax.swing.JCheckBox();
        tagAddPointsCheckBox = new javax.swing.JCheckBox();
        tagAddPointsTextField = new javax.swing.JTextField();
        tagSetAmmoCheckBox = new javax.swing.JCheckBox();
        tagSetAmmoTextField = new javax.swing.JTextField();
        tagSetHealthCheckBox = new javax.swing.JCheckBox();
        tagSetHelathTextField = new javax.swing.JTextField();
        tagAddButton = new javax.swing.JButton();
        tagCancelButton = new javax.swing.JButton();
        tagDeleteButton = new javax.swing.JButton();
        playerListPanel1 = new javax.swing.JPanel();
        tagListScrollPane = new javax.swing.JScrollPane();
        tagListTable = new javax.swing.JTable();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("LaserTag Server");
        setPreferredSize(new java.awt.Dimension(1300, 700));

        mainTabbedPane.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                mainTabbedPaneFocusGained(evt);
            }
        });

        overviewPanel.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentShown(java.awt.event.ComponentEvent evt) {
                overviewPanelComponentShown(evt);
            }
        });
        overviewPanel.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                overviewPanelFocusGained(evt);
            }
        });

        statisticsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Current Statistics", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        firstTeamStatPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "1. Team - Red", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        fristTeamStatTable.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        fristTeamStatTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null}
            },
            new String [] {
                "Title 1", "Title 2", "Title 3", "Title 4"
            }
        ));
        fristTeamStatScrollPane.setViewportView(fristTeamStatTable);

        firstTeamScorePanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Overall Score", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        firstTeamScoreLabel.setFont(new java.awt.Font("Tahoma", 1, 18)); // NOI18N
        firstTeamScoreLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        firstTeamScoreLabel.setText("0");

        javax.swing.GroupLayout firstTeamScorePanelLayout = new javax.swing.GroupLayout(firstTeamScorePanel);
        firstTeamScorePanel.setLayout(firstTeamScorePanelLayout);
        firstTeamScorePanelLayout.setHorizontalGroup(
            firstTeamScorePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(firstTeamScorePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(firstTeamScoreLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        firstTeamScorePanelLayout.setVerticalGroup(
            firstTeamScorePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(firstTeamScorePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(firstTeamScoreLabel)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout firstTeamStatPanelLayout = new javax.swing.GroupLayout(firstTeamStatPanel);
        firstTeamStatPanel.setLayout(firstTeamStatPanelLayout);
        firstTeamStatPanelLayout.setHorizontalGroup(
            firstTeamStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(firstTeamStatPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(firstTeamStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(fristTeamStatScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 616, Short.MAX_VALUE)
                    .addComponent(firstTeamScorePanel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        firstTeamStatPanelLayout.setVerticalGroup(
            firstTeamStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(firstTeamStatPanelLayout.createSequentialGroup()
                .addComponent(fristTeamStatScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 243, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(firstTeamScorePanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        secondTeamStatPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "2. Team - Blue", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        secondTeamStatTable.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        secondTeamStatTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null}
            },
            new String [] {
                "Title 1", "Title 2", "Title 3", "Title 4"
            }
        ));
        secondTeamStatScrollPane.setViewportView(secondTeamStatTable);

        secondTeamScorePanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Overall Score", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        secondTeamScoreLabel.setFont(new java.awt.Font("Tahoma", 1, 18)); // NOI18N
        secondTeamScoreLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        secondTeamScoreLabel.setText("0");

        javax.swing.GroupLayout secondTeamScorePanelLayout = new javax.swing.GroupLayout(secondTeamScorePanel);
        secondTeamScorePanel.setLayout(secondTeamScorePanelLayout);
        secondTeamScorePanelLayout.setHorizontalGroup(
            secondTeamScorePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(secondTeamScorePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(secondTeamScoreLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        secondTeamScorePanelLayout.setVerticalGroup(
            secondTeamScorePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(secondTeamScorePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(secondTeamScoreLabel)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout secondTeamStatPanelLayout = new javax.swing.GroupLayout(secondTeamStatPanel);
        secondTeamStatPanel.setLayout(secondTeamStatPanelLayout);
        secondTeamStatPanelLayout.setHorizontalGroup(
            secondTeamStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(secondTeamStatPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(secondTeamStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(secondTeamStatScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 616, Short.MAX_VALUE)
                    .addComponent(secondTeamScorePanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        secondTeamStatPanelLayout.setVerticalGroup(
            secondTeamStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(secondTeamStatPanelLayout.createSequentialGroup()
                .addComponent(secondTeamStatScrollPane, javax.swing.GroupLayout.PREFERRED_SIZE, 0, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(secondTeamScorePanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        javax.swing.GroupLayout statisticsPanelLayout = new javax.swing.GroupLayout(statisticsPanel);
        statisticsPanel.setLayout(statisticsPanelLayout);
        statisticsPanelLayout.setHorizontalGroup(
            statisticsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(statisticsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(firstTeamStatPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(secondTeamStatPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        statisticsPanelLayout.setVerticalGroup(
            statisticsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(statisticsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(statisticsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(firstTeamStatPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(secondTeamStatPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );

        topPlayerPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Top Player", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        topPlayerList.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        topPlayerList.setModel(new javax.swing.AbstractListModel() {
            String[] strings = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
            public int getSize() { return strings.length; }
            public Object getElementAt(int i) { return strings[i]; }
        });
        topPlayerScrollPane.setViewportView(topPlayerList);

        javax.swing.GroupLayout topPlayerPanelLayout = new javax.swing.GroupLayout(topPlayerPanel);
        topPlayerPanel.setLayout(topPlayerPanelLayout);
        topPlayerPanelLayout.setHorizontalGroup(
            topPlayerPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(topPlayerPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(topPlayerScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 329, Short.MAX_VALUE)
                .addContainerGap())
        );
        topPlayerPanelLayout.setVerticalGroup(
            topPlayerPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(topPlayerPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(topPlayerScrollPane)
                .addContainerGap())
        );

        GameStatPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Game Stats", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        gameModeTitelLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        gameModeTitelLabel.setText("Game-Mode:");

        gameModeInputLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        gameModeInputLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        gameModeInputLabel.setText("2 Teams");

        playedGamesTitelLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playedGamesTitelLabel.setText("Played Games:");

        playedGamesInputLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playedGamesInputLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        playedGamesInputLabel.setText("1");

        remainingTimeTitleLabel.setFont(new java.awt.Font("Tahoma", 0, 18)); // NOI18N
        remainingTimeTitleLabel.setText("Running Time:");

        remainingTimeInputLabel.setFont(new java.awt.Font("Tahoma", 0, 18)); // NOI18N
        remainingTimeInputLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        remainingTimeInputLabel.setText("00:00");

        gameStatusLabel.setFont(new java.awt.Font("Tahoma", 0, 18)); // NOI18N
        gameStatusLabel.setText("Game Status:");

        gameStatusInputLabel.setFont(new java.awt.Font("Tahoma", 0, 18)); // NOI18N
        gameStatusInputLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        gameStatusInputLabel.setText("Game stopped!");

        javax.swing.GroupLayout GameStatPanelLayout = new javax.swing.GroupLayout(GameStatPanel);
        GameStatPanel.setLayout(GameStatPanelLayout);
        GameStatPanelLayout.setHorizontalGroup(
            GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(GameStatPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(GameStatPanelLayout.createSequentialGroup()
                        .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(playedGamesTitelLabel)
                            .addComponent(gameModeTitelLabel))
                        .addGap(46, 46, 46)
                        .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(playedGamesInputLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(gameModeInputLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 397, Short.MAX_VALUE)))
                    .addGroup(GameStatPanelLayout.createSequentialGroup()
                        .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(remainingTimeTitleLabel)
                            .addComponent(gameStatusLabel))
                        .addGap(35, 35, 35)
                        .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(remainingTimeInputLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(gameStatusInputLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))))
                .addContainerGap())
        );
        GameStatPanelLayout.setVerticalGroup(
            GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(GameStatPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(gameModeTitelLabel)
                    .addComponent(gameModeInputLabel))
                .addGap(18, 18, 18)
                .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(playedGamesTitelLabel)
                    .addComponent(playedGamesInputLabel))
                .addGap(28, 28, 28)
                .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(remainingTimeTitleLabel)
                    .addComponent(remainingTimeInputLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGroup(GameStatPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(gameStatusLabel)
                    .addComponent(gameStatusInputLabel))
                .addContainerGap())
        );

        gameControlPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Game Control", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        startGameButton.setFont(new java.awt.Font("Tahoma", 1, 16)); // NOI18N
        startGameButton.setText("Start Game");
        startGameButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                startGameButtonActionPerformed(evt);
            }
        });

        stopGameButton.setFont(new java.awt.Font("Tahoma", 1, 16)); // NOI18N
        stopGameButton.setText("Stop Game");
        stopGameButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                stopGameButtonActionPerformed(evt);
            }
        });

        pauseGameButton.setFont(new java.awt.Font("Tahoma", 1, 16)); // NOI18N
        pauseGameButton.setText("Pause Game");
        pauseGameButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                pauseGameButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout gameControlPanelLayout = new javax.swing.GroupLayout(gameControlPanel);
        gameControlPanel.setLayout(gameControlPanelLayout);
        gameControlPanelLayout.setHorizontalGroup(
            gameControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(gameControlPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(gameControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(startGameButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(stopGameButton, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(pauseGameButton, javax.swing.GroupLayout.DEFAULT_SIZE, 349, Short.MAX_VALUE))
                .addContainerGap())
        );
        gameControlPanelLayout.setVerticalGroup(
            gameControlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(gameControlPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(startGameButton, javax.swing.GroupLayout.PREFERRED_SIZE, 45, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(stopGameButton, javax.swing.GroupLayout.PREFERRED_SIZE, 45, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(pauseGameButton, javax.swing.GroupLayout.PREFERRED_SIZE, 45, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout overviewPanelLayout = new javax.swing.GroupLayout(overviewPanel);
        overviewPanel.setLayout(overviewPanelLayout);
        overviewPanelLayout.setHorizontalGroup(
            overviewPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(overviewPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(overviewPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(statisticsPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(overviewPanelLayout.createSequentialGroup()
                        .addComponent(topPlayerPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(GameStatPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(gameControlPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap())
        );
        overviewPanelLayout.setVerticalGroup(
            overviewPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(overviewPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(statisticsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(overviewPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(GameStatPanel, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(topPlayerPanel, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(gameControlPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        mainTabbedPane.addTab("Overview", overviewPanel);

        gameModePanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Game Mode", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        gameModeLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        gameModeLabel.setText("Mode:");

        gameModeComboBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        gameModeComboBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "2 Teams" }));

        javax.swing.GroupLayout gameModePanelLayout = new javax.swing.GroupLayout(gameModePanel);
        gameModePanel.setLayout(gameModePanelLayout);
        gameModePanelLayout.setHorizontalGroup(
            gameModePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(gameModePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(gameModeLabel)
                .addGap(18, 18, 18)
                .addComponent(gameModeComboBox, 0, 336, Short.MAX_VALUE)
                .addContainerGap())
        );
        gameModePanelLayout.setVerticalGroup(
            gameModePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(gameModePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(gameModePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(gameModeLabel)
                    .addComponent(gameModeComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        setPlayTimePanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Play Time", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        setPlayTimeLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        setPlayTimeLabel.setText("Play Time:");

        setPlayTimeTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        setPlayTimeTextField.setText("12");

        playTimeMinLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playTimeMinLabel.setText("min");

        javax.swing.GroupLayout setPlayTimePanelLayout = new javax.swing.GroupLayout(setPlayTimePanel);
        setPlayTimePanel.setLayout(setPlayTimePanelLayout);
        setPlayTimePanelLayout.setHorizontalGroup(
            setPlayTimePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(setPlayTimePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(setPlayTimeLabel)
                .addGap(18, 18, 18)
                .addComponent(setPlayTimeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 217, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(playTimeMinLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 85, Short.MAX_VALUE)
                .addContainerGap())
        );
        setPlayTimePanelLayout.setVerticalGroup(
            setPlayTimePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(setPlayTimePanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(setPlayTimePanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(setPlayTimeLabel)
                    .addComponent(setPlayTimeTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(playTimeMinLabel))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        setGameSettingsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Game Settings", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        maxAmmoLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        maxAmmoLabel.setText("Maximum Ammunition:");

        maxHealthLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        maxHealthLabel.setText("Maximum Health:");

        pointsPerTagLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        pointsPerTagLabel.setText("Points-per-Tag:");

        ammoPerTagLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        ammoPerTagLabel.setText("Ammunition-per-Tag:");

        healthPerTagLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        healthPerTagLabel.setText("Health-per-Tag:");

        maxAmmoTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        maxAmmoTextField.setText("100");

        maxHealthTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        maxHealthTextField.setText("100");

        pointsPerTagTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        pointsPerTagTextField.setText("50");

        ammoPerTagTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        ammoPerTagTextField.setText("5");

        healthPerTagTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        healthPerTagTextField.setText("5");

        javax.swing.GroupLayout setGameSettingsPanelLayout = new javax.swing.GroupLayout(setGameSettingsPanel);
        setGameSettingsPanel.setLayout(setGameSettingsPanelLayout);
        setGameSettingsPanelLayout.setHorizontalGroup(
            setGameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(setGameSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(setGameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(setGameSettingsPanelLayout.createSequentialGroup()
                        .addComponent(maxAmmoLabel)
                        .addGap(18, 18, 18)
                        .addComponent(maxAmmoTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 238, Short.MAX_VALUE))
                    .addGroup(setGameSettingsPanelLayout.createSequentialGroup()
                        .addComponent(healthPerTagLabel)
                        .addGap(73, 73, 73)
                        .addComponent(healthPerTagTextField))
                    .addGroup(setGameSettingsPanelLayout.createSequentialGroup()
                        .addComponent(maxHealthLabel)
                        .addGap(61, 61, 61)
                        .addComponent(maxHealthTextField))
                    .addGroup(setGameSettingsPanelLayout.createSequentialGroup()
                        .addComponent(pointsPerTagLabel)
                        .addGap(75, 75, 75)
                        .addComponent(pointsPerTagTextField))
                    .addGroup(setGameSettingsPanelLayout.createSequentialGroup()
                        .addComponent(ammoPerTagLabel)
                        .addGap(30, 30, 30)
                        .addComponent(ammoPerTagTextField)))
                .addContainerGap())
        );
        setGameSettingsPanelLayout.setVerticalGroup(
            setGameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(setGameSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(setGameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(maxAmmoLabel)
                    .addComponent(maxAmmoTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(setGameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(maxHealthLabel)
                    .addComponent(maxHealthTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(setGameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(pointsPerTagLabel)
                    .addComponent(pointsPerTagTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(setGameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(ammoPerTagLabel)
                    .addComponent(ammoPerTagTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(setGameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(healthPerTagLabel)
                    .addComponent(healthPerTagTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        teamSettingsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Team Settings", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        firstTeamSettingsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "1. Team", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        firstTeamColorLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        firstTeamColorLabel.setText("Color:");

        firstTeamColorComboBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        firstTeamColorComboBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Red", "Green", "Blue" }));

        firstTeamNameLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        firstTeamNameLabel.setText("Name:");

        firstTeamNameTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        firstTeamNameTextField.setText("Team1");

        javax.swing.GroupLayout firstTeamSettingsPanelLayout = new javax.swing.GroupLayout(firstTeamSettingsPanel);
        firstTeamSettingsPanel.setLayout(firstTeamSettingsPanelLayout);
        firstTeamSettingsPanelLayout.setHorizontalGroup(
            firstTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(firstTeamSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(firstTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(firstTeamNameLabel)
                    .addComponent(firstTeamColorLabel))
                .addGap(18, 18, 18)
                .addGroup(firstTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(firstTeamColorComboBox, 0, 320, Short.MAX_VALUE)
                    .addComponent(firstTeamNameTextField))
                .addContainerGap())
        );
        firstTeamSettingsPanelLayout.setVerticalGroup(
            firstTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(firstTeamSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(firstTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(firstTeamColorLabel)
                    .addComponent(firstTeamColorComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(firstTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(firstTeamNameLabel)
                    .addComponent(firstTeamNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        secondTeamSettingsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "2. Team", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        secondTeamColorComboBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        secondTeamColorComboBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Red", "Green", "Blue" }));

        secondTeamColorLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        secondTeamColorLabel.setText("Color:");

        secondTeamNameLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        secondTeamNameLabel.setText("Name:");

        secondTeamNameTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        secondTeamNameTextField.setText("Team2");

        javax.swing.GroupLayout secondTeamSettingsPanelLayout = new javax.swing.GroupLayout(secondTeamSettingsPanel);
        secondTeamSettingsPanel.setLayout(secondTeamSettingsPanelLayout);
        secondTeamSettingsPanelLayout.setHorizontalGroup(
            secondTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(secondTeamSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(secondTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(secondTeamNameLabel)
                    .addComponent(secondTeamColorLabel))
                .addGap(18, 18, 18)
                .addGroup(secondTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(secondTeamColorComboBox, 0, 320, Short.MAX_VALUE)
                    .addComponent(secondTeamNameTextField))
                .addContainerGap())
        );
        secondTeamSettingsPanelLayout.setVerticalGroup(
            secondTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(secondTeamSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(secondTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(secondTeamColorLabel)
                    .addComponent(secondTeamColorComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(secondTeamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(secondTeamNameLabel)
                    .addComponent(secondTeamNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout teamSettingsPanelLayout = new javax.swing.GroupLayout(teamSettingsPanel);
        teamSettingsPanel.setLayout(teamSettingsPanelLayout);
        teamSettingsPanelLayout.setHorizontalGroup(
            teamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(teamSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(firstTeamSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(secondTeamSettingsPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        teamSettingsPanelLayout.setVerticalGroup(
            teamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, teamSettingsPanelLayout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGroup(teamSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(secondTeamSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(firstTeamSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );

        saveGameSettingsButton.setFont(new java.awt.Font("Tahoma", 0, 18)); // NOI18N
        saveGameSettingsButton.setText("Save");
        saveGameSettingsButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveGameSettingsButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout gameSettingsPanelLayout = new javax.swing.GroupLayout(gameSettingsPanel);
        gameSettingsPanel.setLayout(gameSettingsPanelLayout);
        gameSettingsPanelLayout.setHorizontalGroup(
            gameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(gameSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(gameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(saveGameSettingsButton, javax.swing.GroupLayout.PREFERRED_SIZE, 141, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(gameSettingsPanelLayout.createSequentialGroup()
                        .addGroup(gameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(gameSettingsPanelLayout.createSequentialGroup()
                                .addComponent(gameModePanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                .addComponent(setPlayTimePanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addComponent(teamSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(setGameSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap())
        );
        gameSettingsPanelLayout.setVerticalGroup(
            gameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(gameSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(gameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(setGameSettingsPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(gameSettingsPanelLayout.createSequentialGroup()
                        .addGroup(gameSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(gameModePanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(setPlayTimePanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(teamSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addGap(18, 18, 18)
                .addComponent(saveGameSettingsButton, javax.swing.GroupLayout.PREFERRED_SIZE, 49, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(337, Short.MAX_VALUE))
        );

        mainTabbedPane.addTab("Game-Settings", gameSettingsPanel);

        playerPanel.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                playerPanelFocusGained(evt);
            }
        });

        playerSettingsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "New Player", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        playerGeneralPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "General", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        playerNumberLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerNumberLabel.setText("Player-Number:");

        playerNumberTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerNumberTextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        playerNumberTextField.setText("0");
        playerNumberTextField.setEnabled(false);

        playerNameLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerNameLabel.setText("Player-Name:");

        playerNameTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N

        vestNumberLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        vestNumberLabel.setText("Vest-Number:");

        vestNumberTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N

        playerTeamLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerTeamLabel.setText("Team:");

        playerTeamComboBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N

        taggerTypButtonGroup.add(edisonRadioButton);
        edisonRadioButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        edisonRadioButton.setSelected(true);
        edisonRadioButton.setText("Edison");

        taggerTypButtonGroup.add(arduinoRadioButton);
        arduinoRadioButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        arduinoRadioButton.setText("Arduino");

        javax.swing.GroupLayout playerGeneralPanelLayout = new javax.swing.GroupLayout(playerGeneralPanel);
        playerGeneralPanel.setLayout(playerGeneralPanelLayout);
        playerGeneralPanelLayout.setHorizontalGroup(
            playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerGeneralPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(playerNumberLabel)
                    .addComponent(playerTeamLabel)
                    .addComponent(playerNameLabel)
                    .addComponent(vestNumberLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(playerTeamComboBox, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(playerNameTextField, javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, playerGeneralPanelLayout.createSequentialGroup()
                        .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(vestNumberTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 81, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(playerNumberTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 81, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(edisonRadioButton)
                            .addComponent(arduinoRadioButton))))
                .addContainerGap())
        );
        playerGeneralPanelLayout.setVerticalGroup(
            playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerGeneralPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(playerNumberLabel)
                    .addComponent(playerNumberTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(edisonRadioButton))
                .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(playerGeneralPanelLayout.createSequentialGroup()
                        .addGap(18, 18, 18)
                        .addComponent(arduinoRadioButton)
                        .addGap(18, 18, 18)
                        .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(playerNameLabel)
                            .addComponent(playerNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
                    .addGroup(playerGeneralPanelLayout.createSequentialGroup()
                        .addGap(18, 18, 18)
                        .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(vestNumberLabel)
                            .addComponent(vestNumberTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))))
                .addGap(18, 18, 18)
                .addGroup(playerGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(playerTeamLabel)
                    .addComponent(playerTeamComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        playerHealthPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Health", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        playerHelathLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerHelathLabel.setText("Health:");

        playerHelathTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerHelathTextField.setText("100");
        playerHelathTextField.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusLost(java.awt.event.FocusEvent evt) {
                playerHelathTextFieldFocusLost(evt);
            }
        });

        playerHealthProgressBar.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N

        javax.swing.GroupLayout playerHealthPanelLayout = new javax.swing.GroupLayout(playerHealthPanel);
        playerHealthPanel.setLayout(playerHealthPanelLayout);
        playerHealthPanelLayout.setHorizontalGroup(
            playerHealthPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerHealthPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerHealthPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(playerHealthProgressBar, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(playerHealthPanelLayout.createSequentialGroup()
                        .addComponent(playerHelathLabel)
                        .addGap(81, 81, 81)
                        .addComponent(playerHelathTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 81, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(0, 0, Short.MAX_VALUE)))
                .addContainerGap())
        );
        playerHealthPanelLayout.setVerticalGroup(
            playerHealthPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerHealthPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerHealthPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(playerHelathLabel)
                    .addComponent(playerHelathTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addComponent(playerHealthProgressBar, javax.swing.GroupLayout.DEFAULT_SIZE, 22, Short.MAX_VALUE)
                .addContainerGap())
        );

        playerAmmoPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Ammunition", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        playerAmmoLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerAmmoLabel.setText("Ammunition:");

        playerAmmoTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerAmmoTextField.setText("100");
        playerAmmoTextField.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusLost(java.awt.event.FocusEvent evt) {
                playerAmmoTextFieldFocusLost(evt);
            }
        });

        javax.swing.GroupLayout playerAmmoPanelLayout = new javax.swing.GroupLayout(playerAmmoPanel);
        playerAmmoPanel.setLayout(playerAmmoPanelLayout);
        playerAmmoPanelLayout.setHorizontalGroup(
            playerAmmoPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerAmmoPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerAmmoPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(playerAmmoProgressBar, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(playerAmmoPanelLayout.createSequentialGroup()
                        .addComponent(playerAmmoLabel)
                        .addGap(38, 38, 38)
                        .addComponent(playerAmmoTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 81, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(0, 0, Short.MAX_VALUE)))
                .addContainerGap())
        );
        playerAmmoPanelLayout.setVerticalGroup(
            playerAmmoPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerAmmoPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerAmmoPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(playerAmmoLabel)
                    .addComponent(playerAmmoTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addComponent(playerAmmoProgressBar, javax.swing.GroupLayout.PREFERRED_SIZE, 22, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        playerPointsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Points", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        playerPointsLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerPointsLabel.setText("Points:");

        playerPointsTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerPointsTextField.setText("0");

        javax.swing.GroupLayout playerPointsPanelLayout = new javax.swing.GroupLayout(playerPointsPanel);
        playerPointsPanel.setLayout(playerPointsPanelLayout);
        playerPointsPanelLayout.setHorizontalGroup(
            playerPointsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerPointsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(playerPointsLabel)
                .addGap(85, 85, 85)
                .addComponent(playerPointsTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 81, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        playerPointsPanelLayout.setVerticalGroup(
            playerPointsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerPointsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerPointsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(playerPointsLabel)
                    .addComponent(playerPointsTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        playerAddButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerAddButton.setText("Add Player");
        playerAddButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                playerAddButtonActionPerformed(evt);
            }
        });

        playerCancelButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerCancelButton.setText("Cancel");
        playerCancelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                playerCancelButtonActionPerformed(evt);
            }
        });

        playerDeleteButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playerDeleteButton.setText("Delete Player");
        playerDeleteButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                playerDeleteButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout playerSettingsPanelLayout = new javax.swing.GroupLayout(playerSettingsPanel);
        playerSettingsPanel.setLayout(playerSettingsPanelLayout);
        playerSettingsPanelLayout.setHorizontalGroup(
            playerSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(playerGeneralPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(playerHealthPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(playerAmmoPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(playerPointsPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, playerSettingsPanelLayout.createSequentialGroup()
                        .addComponent(playerCancelButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 50, Short.MAX_VALUE)
                        .addComponent(playerDeleteButton)
                        .addGap(18, 18, 18)
                        .addComponent(playerAddButton)))
                .addContainerGap())
        );
        playerSettingsPanelLayout.setVerticalGroup(
            playerSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(playerGeneralPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(playerHealthPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(playerAmmoPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(playerPointsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(playerSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(playerSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(playerAddButton)
                        .addComponent(playerDeleteButton))
                    .addComponent(playerCancelButton))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        playerListPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Players", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        playersListTable.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        playersListTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null}
            },
            new String [] {
                "Title 1", "Title 2", "Title 3", "Title 4"
            }
        ));
        playersListScrollPane.setViewportView(playersListTable);

        javax.swing.GroupLayout playerListPanelLayout = new javax.swing.GroupLayout(playerListPanel);
        playerListPanel.setLayout(playerListPanelLayout);
        playerListPanelLayout.setHorizontalGroup(
            playerListPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerListPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(playersListScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 883, Short.MAX_VALUE)
                .addContainerGap())
        );
        playerListPanelLayout.setVerticalGroup(
            playerListPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerListPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(playersListScrollPane)
                .addContainerGap())
        );

        javax.swing.GroupLayout playerPanelLayout = new javax.swing.GroupLayout(playerPanel);
        playerPanel.setLayout(playerPanelLayout);
        playerPanelLayout.setHorizontalGroup(
            playerPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(playerSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(playerListPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        playerPanelLayout.setVerticalGroup(
            playerPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(playerPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(playerListPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(playerSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, 631, Short.MAX_VALUE))
                .addContainerGap(37, Short.MAX_VALUE))
        );

        mainTabbedPane.addTab("Add/Edit Players", playerPanel);

        rfidPanel.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                rfidPanelFocusGained(evt);
            }
        });

        tagSettingsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "New Tag", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        tagGeneralPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "General", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        tagNumberLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagNumberLabel.setText("Tag-Number:");

        tagNumberTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagNumberTextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        tagNumberTextField.setText("0");
        tagNumberTextField.setEnabled(false);

        tagUIDLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagUIDLabel.setText("UID:");

        tagUIDTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagUIDTextField.setEnabled(false);

        tagNameLabel.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagNameLabel.setText("Name/Info:");

        tagNameTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N

        newTagButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        newTagButton.setText("New Tag");
        newTagButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                newTagButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout tagGeneralPanelLayout = new javax.swing.GroupLayout(tagGeneralPanel);
        tagGeneralPanel.setLayout(tagGeneralPanelLayout);
        tagGeneralPanelLayout.setHorizontalGroup(
            tagGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(tagGeneralPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(tagGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(tagGeneralPanelLayout.createSequentialGroup()
                        .addGroup(tagGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(tagNumberLabel)
                            .addComponent(tagUIDLabel)
                            .addComponent(tagNameLabel))
                        .addGap(18, 18, 18)
                        .addGroup(tagGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(tagUIDTextField)
                            .addGroup(tagGeneralPanelLayout.createSequentialGroup()
                                .addComponent(tagNumberTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 81, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addGap(0, 150, Short.MAX_VALUE))
                            .addComponent(tagNameTextField)))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, tagGeneralPanelLayout.createSequentialGroup()
                        .addGap(0, 0, Short.MAX_VALUE)
                        .addComponent(newTagButton)))
                .addContainerGap())
        );
        tagGeneralPanelLayout.setVerticalGroup(
            tagGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(tagGeneralPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(tagGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(tagNumberLabel)
                    .addComponent(tagNumberTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(tagGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(tagUIDLabel)
                    .addComponent(tagUIDTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(tagGeneralPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(tagNameLabel)
                    .addComponent(tagNameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(newTagButton)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        tagFunctionPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Function", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 1, 16))); // NOI18N

        tagReloadAmmoCheckBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagReloadAmmoCheckBox.setText("Reload Ammunition");

        tagReloadHealthCheckBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagReloadHealthCheckBox.setText("Reload Health");

        tagAddPointsCheckBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagAddPointsCheckBox.setText("Add Points:");

        tagAddPointsTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagAddPointsTextField.setText("50");

        tagSetAmmoCheckBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagSetAmmoCheckBox.setText("Set Ammunition to:");

        tagSetAmmoTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagSetAmmoTextField.setText("80");

        tagSetHealthCheckBox.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagSetHealthCheckBox.setText("Set Health to:");

        tagSetHelathTextField.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagSetHelathTextField.setText("80");

        javax.swing.GroupLayout tagFunctionPanelLayout = new javax.swing.GroupLayout(tagFunctionPanel);
        tagFunctionPanel.setLayout(tagFunctionPanelLayout);
        tagFunctionPanelLayout.setHorizontalGroup(
            tagFunctionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(tagFunctionPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(tagFunctionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(tagReloadAmmoCheckBox)
                    .addComponent(tagReloadHealthCheckBox)
                    .addGroup(tagFunctionPanelLayout.createSequentialGroup()
                        .addComponent(tagAddPointsCheckBox)
                        .addGap(74, 74, 74)
                        .addComponent(tagAddPointsTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 92, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(tagFunctionPanelLayout.createSequentialGroup()
                        .addGroup(tagFunctionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(tagSetAmmoCheckBox)
                            .addComponent(tagSetHealthCheckBox))
                        .addGap(18, 18, 18)
                        .addGroup(tagFunctionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(tagSetAmmoTextField)
                            .addComponent(tagSetHelathTextField, javax.swing.GroupLayout.DEFAULT_SIZE, 92, Short.MAX_VALUE))))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        tagFunctionPanelLayout.setVerticalGroup(
            tagFunctionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(tagFunctionPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(tagReloadAmmoCheckBox)
                .addGap(18, 18, 18)
                .addComponent(tagReloadHealthCheckBox)
                .addGap(18, 18, 18)
                .addGroup(tagFunctionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(tagAddPointsCheckBox)
                    .addComponent(tagAddPointsTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(tagFunctionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(tagSetHealthCheckBox)
                    .addComponent(tagSetHelathTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(tagFunctionPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(tagSetAmmoCheckBox)
                    .addComponent(tagSetAmmoTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        tagAddButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagAddButton.setText("Add Tag");
        tagAddButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                tagAddButtonActionPerformed(evt);
            }
        });

        tagCancelButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagCancelButton.setText("Cancel");
        tagCancelButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                tagCancelButtonActionPerformed(evt);
            }
        });

        tagDeleteButton.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagDeleteButton.setText("Delete Tag");
        tagDeleteButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                tagDeleteButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout tagSettingsPanelLayout = new javax.swing.GroupLayout(tagSettingsPanel);
        tagSettingsPanel.setLayout(tagSettingsPanelLayout);
        tagSettingsPanelLayout.setHorizontalGroup(
            tagSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(tagSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(tagSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(tagGeneralPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(tagFunctionPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, tagSettingsPanelLayout.createSequentialGroup()
                        .addComponent(tagCancelButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 78, Short.MAX_VALUE)
                        .addComponent(tagDeleteButton)
                        .addGap(18, 18, 18)
                        .addComponent(tagAddButton)))
                .addContainerGap())
        );
        tagSettingsPanelLayout.setVerticalGroup(
            tagSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(tagSettingsPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(tagGeneralPanel, javax.swing.GroupLayout.PREFERRED_SIZE, 203, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(tagFunctionPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addGroup(tagSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(tagSettingsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(tagAddButton)
                        .addComponent(tagDeleteButton))
                    .addComponent(tagCancelButton))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        playerListPanel1.setBorder(javax.swing.BorderFactory.createTitledBorder(null, "Tags", javax.swing.border.TitledBorder.DEFAULT_JUSTIFICATION, javax.swing.border.TitledBorder.DEFAULT_POSITION, new java.awt.Font("Tahoma", 0, 18))); // NOI18N

        tagListTable.setFont(new java.awt.Font("Tahoma", 0, 16)); // NOI18N
        tagListTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null}
            },
            new String [] {
                "Title 1", "Title 2", "Title 3", "Title 4"
            }
        ));
        tagListScrollPane.setViewportView(tagListTable);

        javax.swing.GroupLayout playerListPanel1Layout = new javax.swing.GroupLayout(playerListPanel1);
        playerListPanel1.setLayout(playerListPanel1Layout);
        playerListPanel1Layout.setHorizontalGroup(
            playerListPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerListPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(tagListScrollPane)
                .addContainerGap())
        );
        playerListPanel1Layout.setVerticalGroup(
            playerListPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(playerListPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(tagListScrollPane)
                .addContainerGap())
        );

        javax.swing.GroupLayout rfidPanelLayout = new javax.swing.GroupLayout(rfidPanel);
        rfidPanel.setLayout(rfidPanelLayout);
        rfidPanelLayout.setHorizontalGroup(
            rfidPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(rfidPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(tagSettingsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(playerListPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
        rfidPanelLayout.setVerticalGroup(
            rfidPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(rfidPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(rfidPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(playerListPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(tagSettingsPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap(85, Short.MAX_VALUE))
        );

        mainTabbedPane.addTab("Add/Edit RFID-Tags", rfidPanel);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(mainTabbedPane)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(mainTabbedPane)
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    
    // GUI-INTERACTION METHODS:
    
    /**
     * Handler for a click on the save game settings button. Saves all the
     * settings.
     * 
     * @param evt - the click event.
     */
    private void saveGameSettingsButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveGameSettingsButtonActionPerformed
        // Get and save the settings.
        gameMode = this.gameModeComboBox.getSelectedIndex();
        firstTeamColor = this.firstTeamColorComboBox.getSelectedIndex();
        secondTeamColor = this.secondTeamColorComboBox.getSelectedIndex();
        firstTeamName = this.firstTeamNameTextField.getText();
        secondTeamName = this.secondTeamNameTextField.getText();
        maxAmmu = Integer.parseInt(this.maxAmmoTextField.getText());
        maxHealth = Integer.parseInt(this.maxHealthTextField.getText());
        pointsPerTag = Integer.parseInt(this.pointsPerTagTextField.getText());
        ammuPerTag = Integer.parseInt(this.ammoPerTagTextField.getText());
        healthPerTag = Integer.parseInt(this.healthPerTagTextField.getText());
        playTime = Integer.parseInt(this.setPlayTimeTextField.getText()) * 60;
        // Set the teams in team selection combobox in the add player section.
        this.playerTeamComboBox.removeAllItems();
        String firstTeamItem = firstTeamName + " - " + colors[firstTeamColor];
        this.playerTeamComboBox.addItem(firstTeamItem);
        String secondTeamItem = secondTeamName + " - " + colors[secondTeamColor];
        this.playerTeamComboBox.addItem(secondTeamItem);
        // Set the pabel titel for the team-panels in ther overview section.
        TitledBorder firstTeamPanelBorder = (TitledBorder) this.firstTeamStatPanel.getBorder();
        firstTeamPanelBorder.setTitle(firstTeamItem);
        this.firstTeamStatPanel.setBorder(firstTeamPanelBorder);
        TitledBorder secondTeamPanelBorder = (TitledBorder) this.secondTeamStatPanel.getBorder();
        secondTeamPanelBorder.setTitle(secondTeamItem);
        this.secondTeamStatPanel.setBorder(secondTeamPanelBorder);
    }//GEN-LAST:event_saveGameSettingsButtonActionPerformed

    /**
     * Handler for a focus loss on the health field for a player. Adjustes the
     * progress-bar.
     * 
     * @param evt - the event.
     */
    private void playerHelathTextFieldFocusLost(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_playerHelathTextFieldFocusLost
        // Get the health value, limit it and adjust the progress bar.
        int health = Integer.parseInt(this.playerHelathTextField.getText());
        if (health > maxHealth) health = maxHealth;
        int healthBarValue = health / (maxHealth / 100);
        this.playerHealthProgressBar.setValue(healthBarValue);
    }//GEN-LAST:event_playerHelathTextFieldFocusLost

    /**
     * Handler for a focus loss on the ammunition field for a player. Adjustes the
     * progress-bar.
     * 
     * @param evt - the event.
     */
    private void playerAmmoTextFieldFocusLost(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_playerAmmoTextFieldFocusLost
        // Get the ammunition value, limit it and adjust the progress bar.
        int ammu = Integer.parseInt(this.playerAmmoTextField.getText());
        if (ammu > maxAmmu) ammu = maxAmmu;
        int ammuBarValue = ammu / (maxAmmu / 100);
        this.playerAmmoProgressBar.setValue(ammuBarValue);
    }//GEN-LAST:event_playerAmmoTextFieldFocusLost

    /**
     * Handler for a click on the add/edit player butten button. Saves the new
     * player/the changes.
     * 
     * @param evt - the click event.
     */
    private void playerAddButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_playerAddButtonActionPerformed
        // If a new player should be add.
        if (this.playerAddButton.getText().equals("Add Player")) {
            // Save the current player properties.
            currentVestNumber = Integer.parseInt(this.vestNumberTextField.getText());
            currentPlayerName = this.playerNameTextField.getText();
            currentTeam = this.playerTeamComboBox.getSelectedIndex();
            currentHealth = Integer.parseInt(this.playerHelathTextField.getText());
            currentAmmu = Integer.parseInt(this.playerAmmoTextField.getText());
            currentPoints = Integer.parseInt(this.playerPointsTextField.getText());
            currentTagger = this.edisonRadioButton.isSelected();
            // Wait for a connection.
            cw = new ConnectionWaiter(this, serverSocket);
            cw.start();
            ww = new WaitWindow();
            ww.setVisible(true);
            ww.setDisplayText("Please connect Tagger!");
            ww.setMainWindow(this);
        } else {
            // If a player should be edited, save all changes.
            int pn = Integer.parseInt(this.playerNumberTextField.getText());
            Players.get(pn).health = Integer.parseInt(this.playerHelathTextField.getText());
            Players.get(pn).westNr = Integer.parseInt(this.vestNumberTextField.getText());
            Players.get(pn).name = this.playerNameTextField.getText();
            Players.get(pn).team = this.playerTeamComboBox.getSelectedIndex();
            Players.get(pn).ammu = Integer.parseInt(this.playerAmmoTextField.getText());
            Players.get(pn).points = Integer.parseInt(this.playerPointsTextField.getText());
            Players.get(pn).edisonTagger = this.edisonRadioButton.isSelected();
            if (this.playerTeamComboBox.getSelectedIndex() == 0) {
                Players.get(pn).teamColor = firstTeamColor;
            } else {
                Players.get(pn).teamColor = secondTeamColor;
            }
            // Update the player table.
            playersListTable.setModel(new TableModelPlayer(Players));
            playersListTable.doLayout();
            // Send the changes to the player.
            this.sendAmmuSetCmd(pn, Players.get(pn).ammu);
            this.sendHealthSetCmd(pn, Players.get(pn).health);
            this.sendPointsSetCmd(pn, Players.get(pn).points);
        }
    }//GEN-LAST:event_playerAddButtonActionPerformed

    /**
     * Handler for a click on the new tag butten button. Starts the reading of
     * new tag
     * 
     * @param evt - the click event.
     */
    private void newTagButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_newTagButtonActionPerformed
        // Set the current tag-number.
        this.tagNumberTextField.setText("" + currentTagNumber);
        // Start the reading of the uid of new tag.
        waitForSerial = true;
        ww = new WaitWindow();
        ww.setVisible(true);
        ww.setDisplayText("Please scan Tag!");
        ww.setMainWindow(this);
    }//GEN-LAST:event_newTagButtonActionPerformed

    /**
     * Handler for a click on the add/edit tag butten button. Saves the new
     * tag/the changes.
     * 
     * @param evt - the click event.
     */
    private void tagAddButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_tagAddButtonActionPerformed
        int tNo;
        // If a new tag should ne added.
        if (this.tagAddButton.getText().equals("Add Tag")) {
            // Check if the tag already exits.
            tNo = checkUID(currentUID);
            if (tNo == -1) {
                // If not, save the new tag with all the given properties.
                Tag newTag = new Tag();
                newTag.tagNo = currentTagNumber;
                newTag.uid = currentUID;
                newTag.uidHexString = currentUIDHexString;
                newTag.reloadAmmu = this.tagReloadAmmoCheckBox.isSelected();
                newTag.reloadHealth = this.tagReloadHealthCheckBox.isSelected();
                newTag.ammuTo = this.tagSetAmmoCheckBox.isSelected();
                newTag.ammuToValue = Integer.parseInt(this.tagSetAmmoTextField.getText());
                newTag.pointsTo = this.tagAddPointsCheckBox.isSelected();
                newTag.pointsToValue = Integer.parseInt(this.tagAddPointsTextField.getText());
                newTag.healthTo = this.tagSetHealthCheckBox.isSelected();
                newTag.healthToValue = Integer.parseInt(this.tagSetHelathTextField.getText());
                newTag.name = this.tagNameTextField.getText();
                tags.add(newTag);
                currentTagNumber++;
                this.tagNumberTextField.setText("" + currentTagNumber);
                tagListTable.clearSelection();
            } else {
                // If the tag already exists, open it for some editing.
                editTag(tNo);
            }
        } else {
            // If a tag should be edited, save the changes.
            int tn = Integer.parseInt(this.tagNumberTextField.getText());
            tags.get(tn).reloadAmmu = this.tagReloadAmmoCheckBox.isSelected();
            tags.get(tn).reloadHealth = this.tagReloadHealthCheckBox.isSelected();
            tags.get(tn).ammuTo = this.tagSetAmmoCheckBox.isSelected();
            tags.get(tn).ammuToValue = Integer.parseInt(this.tagSetAmmoTextField.getText());
            tags.get(tn).pointsTo = this.tagAddPointsCheckBox.isSelected();
            tags.get(tn).pointsToValue = Integer.parseInt(this.tagAddPointsTextField.getText());
            tags.get(tn).healthTo = this.tagSetHealthCheckBox.isSelected();
            tags.get(tn).healthToValue = Integer.parseInt(this.tagSetHelathTextField.getText());
            tags.get(tn).name = this.tagNameTextField.getText();
        }
        // Update the tag list tabel.
        tagListTable.setModel(new TableModelTags(tags));
        tagListTable.doLayout();
        this.tagAddButton.setEnabled(false);
    }//GEN-LAST:event_tagAddButtonActionPerformed

    /**
     * Handler for a click on the delete tag butten button. Delete the tag.
     * 
     * @param evt - the click event.
     */
    private void tagDeleteButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_tagDeleteButtonActionPerformed
        // Clear the tag tabel selection and get tag number.
        tagListTable.clearSelection();
        int tn = Integer.parseInt(this.tagNumberTextField.getText());
        // Remove the tag and update the tag tabel.
        removeTag(tn);
        tagListTable.setModel(new TableModelTags(tags));
        tagListTable.doLayout();
        currentTagNumber--;
        // Adjust the title of the panel for adding new tags.
        TitledBorder tSP = (TitledBorder) this.tagSettingsPanel.getBorder();
        tSP.setTitle("New Tag");
        tagSettingsPanel.setBorder(tSP);
        repaint();
        // Deactivate the editing buttons.
        this.tagCancelButton.setEnabled(false);
        this.tagDeleteButton.setEnabled(false);
        this.tagAddButton.setEnabled(false);
        this.tagAddButton.setText("Add Tag");
        // Clear the property fields.
        this.tagNumberTextField.setText("" + currentTagNumber);
        this.tagUIDTextField.setText("");
        this.tagNameTextField.setText("");
        this.tagReloadAmmoCheckBox.setSelected(false);
        this.tagReloadHealthCheckBox.setSelected(false);
        this.tagSetAmmoCheckBox.setSelected(false);
        this.tagSetHealthCheckBox.setSelected(false);
        this.tagAddPointsCheckBox.setSelected(false);
        this.tagSetAmmoTextField.setText("80");
        this.tagSetHelathTextField.setText("80");
        this.tagAddPointsTextField.setText("50");
    }//GEN-LAST:event_tagDeleteButtonActionPerformed

    /**
     * Handler for a click on the start game butten button. Starts the game and
     * the game timer.
     * 
     * @param evt - the click event.
     */
    private void startGameButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_startGameButtonActionPerformed
        // Send the start command to all players.
        sendStartGameCmd();
        // If the game was not just paused, reset the timer.
        if (!gamePaused) {
            gamePaused = false;
            timeInSec = 0;
        }
        // Start the timer.
        gameTimer = new Timer();
            gameTimer.scheduleAtFixedRate(new TimerTask() {
		public void run(){
		    timeInSec++;
                    updateTimerOnGui();
		}
	}, 1000, 1000);
        // Update the GUI.
        this.gameStatusInputLabel.setText("Game running!");
    }//GEN-LAST:event_startGameButtonActionPerformed

    /**
     * Handler for a click on the stop game butten button. Stops the game and
     * the game timer.
     * 
     * @param evt - the click event.
     */
    private void stopGameButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_stopGameButtonActionPerformed
        // Send the stop command to all players.
        sendStopGameCmd();
        // Stop the timer.
        if (gameTimer != null) {
            gameTimer.cancel();
        }
        // Increase the played games counter and update the GUI.
        playedGames++;
        this.playedGamesInputLabel.setText("" + playedGames);
        this.gameStatusInputLabel.setText("Game stopped!");
    }//GEN-LAST:event_stopGameButtonActionPerformed

    /**
     * Handler for a click on the pause game butten button. Pauses the game and
     * the game timer.
     * 
     * @param evt - the click event.
     */
    private void pauseGameButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_pauseGameButtonActionPerformed
        // Send the pause command to all players.
        sendPauseGameCmd();
        // Stop the timer.
        if (gameTimer != null) {
            gameTimer.cancel();
        }
        gamePaused = true;
        // Update the GUI.
        this.gameStatusInputLabel.setText("Game paused!");
    }//GEN-LAST:event_pauseGameButtonActionPerformed

    /**
     * Handler for a click on the cancel tag butten button. Cancels the editing
     * of a tag.
     * 
     * @param evt - the click event.
     */
    private void tagCancelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_tagCancelButtonActionPerformed
        // Clear the selection in tags tabel.
        tagListTable.clearSelection();
        // Adjust the title of the panel for adding new tags.
        TitledBorder tSP = (TitledBorder) this.tagSettingsPanel.getBorder();
        tSP.setTitle("New Tag");
        tagSettingsPanel.setBorder(tSP);
        repaint();
        // Deactivate the editing buttons.
        this.tagCancelButton.setEnabled(false);
        this.tagDeleteButton.setEnabled(false);
        this.tagAddButton.setEnabled(false);
        this.tagAddButton.setText("Add Tag");
        // Clear the property flieds.
        this.tagNumberTextField.setText("" + currentTagNumber);
        this.tagUIDTextField.setText("");
        this.tagNameTextField.setText("");
        this.tagReloadAmmoCheckBox.setSelected(false);
        this.tagReloadHealthCheckBox.setSelected(false);
        this.tagSetAmmoCheckBox.setSelected(false);
        this.tagSetHealthCheckBox.setSelected(false);
        this.tagAddPointsCheckBox.setSelected(false);
        this.tagSetAmmoTextField.setText("50");
        this.tagSetHelathTextField.setText("80");
        this.tagAddPointsTextField.setText("80");
    }//GEN-LAST:event_tagCancelButtonActionPerformed

    /**
     * Handler for a click on the cancel player butten button. Cancels the editing
     * of a player.
     * 
     * @param evt - the click event.
     */
    private void playerCancelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_playerCancelButtonActionPerformed
        // Clear the selection in players tabel.
        playersListTable.clearSelection();
        // Adjust the title of the panel for adding new players.
        TitledBorder pSP = (TitledBorder) this.playerSettingsPanel.getBorder();
        pSP.setTitle("New Player");
        tagSettingsPanel.setBorder(pSP);
        repaint();
        // Deactivate the editing buttons.
        this.playerCancelButton.setEnabled(false);
        this.playerDeleteButton.setEnabled(false);
        this.playerAddButton.setEnabled(true);
        this.playerAddButton.setText("Add Player");
        // Clear the property flieds.
        this.playerNumberTextField.setText(""  + currentPlayerNumber);
        this.vestNumberTextField.setText("");
        this.playerNameTextField.setText("");
        this.edisonRadioButton.setSelected(true);
        this.playerPointsTextField.setText("0");
        // Health:
        int health = 100;
        this.playerHelathTextField.setText("" + health);
        if (health > maxHealth) health = maxHealth;
        int healthBarValue = health / (maxHealth / 100);
        this.playerHealthProgressBar.setValue(healthBarValue);
        // Ammunition:
        int ammu = 100;
        this.playerAmmoTextField.setText("" + ammu);
        if (ammu > maxAmmu) ammu = maxAmmu;
        int ammuBarValue = ammu / (maxAmmu / 100);
        this.playerAmmoProgressBar.setValue(ammuBarValue);
    }//GEN-LAST:event_playerCancelButtonActionPerformed

    /**
     * Handler for a click on the delete player butten button. Schedules the
     * deleting of the player.
     * 
     * @param evt - the click event.
     */
    private void playerDeleteButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_playerDeleteButtonActionPerformed
        // Clear the selection in the player tabel.
        playersListTable.clearSelection();
        // Get the number of the player which should be deleted.
        delPlayer = Integer.parseInt(this.playerNumberTextField.getText());
        // Schedule a stop of the read thread an mark there that a player should
        // be deleted afertwards.
        if (readThread != null) {
            readThread.delPlayer = true;
            readThread.running = false;
        }
    }//GEN-LAST:event_playerDeleteButtonActionPerformed

    private void mainTabbedPaneFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_mainTabbedPaneFocusGained
        // TODO add your handling code here:
    }//GEN-LAST:event_mainTabbedPaneFocusGained

    /**
     * Handler for a focus gain of the player panel. Updates the player list.
     * 
     * @param evt - the event.
     */
    private void playerPanelFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_playerPanelFocusGained
        playersListTable.setModel(new TableModelPlayer(Players));
        playersListTable.doLayout();
    }//GEN-LAST:event_playerPanelFocusGained

    /**
     * Handler for a focus gain of the tag panel. Updates the tag list.
     * 
     * @param evt - the event.
     */
    private void rfidPanelFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_rfidPanelFocusGained
        tagListTable.setModel(new TableModelTags(tags));
        tagListTable.doLayout();
    }//GEN-LAST:event_rfidPanelFocusGained

    private void overviewPanelFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_overviewPanelFocusGained

    }//GEN-LAST:event_overviewPanelFocusGained

    /**
     * Handler for a focus gain of the overview panel. Updates the GUI there.
     * 
     * @param evt - the event.
     */
    private void overviewPanelComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_overviewPanelComponentShown
        this.updateTeamLists(true);
        this.updateTeamScore();
        this.updateTopPlayer();
    }//GEN-LAST:event_overviewPanelComponentShown

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
            java.util.logging.Logger.getLogger(LTServer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(LTServer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(LTServer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(LTServer.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>

        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new LTServer().setVisible(true);
            }
        });
    }

    // MEMBERS:
    
    // General:
    private ConnectionWaiter cw;  // The connection waiter thread.
    private WaitWindow ww;  // The wait window.
    private ServerSocket serverSocket;  // The server socket.
    private LT_ServerReadThread readThread;  // The read thread.
    private ArrayList<Player> Players;  // The players list.
    private ArrayList<Tag> tags;  // The tags list.
    private SerialHandler serialHandler;  // The serial handler
    private int delPlayer;  // Indicates a player which should be deleted.
    // Indiactes if a serial input should be considered.
    private boolean waitForSerial;
    
    // Teams:
    private ArrayList<Player> team1;  // Player list of team 1.
    private ArrayList<Player> team2;  // Player list of team 2.
    private int team1Score;  // Score of team 1.
    private int team2Score;  // Score of team 2.
    
    // Settings:
    private int gameMode;  // Number set represents the game-mode: 0 - 2 Teams.
    private int maxAmmu;  // Max ammunition value.
    private int maxHealth;  // Maximum helath value.
    private int pointsPerTag;  // Number of points per sucessfull tag.
    private int ammuPerTag;  // Ammunition which is needed per tag.
    private int healthPerTag;  // Health which is lost per tag.
    private int playTime;  // Time for one game in sec.
    private String firstTeamName;  // Name of the first team.
    private int firstTeamColor;  // Number that represents the first team color. 
    private String secondTeamName;  // Name of the second team.
    private int secondTeamColor;  // Number that represents the 2. team color.
    private String[] colors = {"red", "green", "blue"};  //Possible team colors.
    private byte[][] colorsRGB = {{(byte)255, (byte) 0, (byte) 0}, {(byte) 0,
            (byte) 255, (byte) 0}, {(byte) 0, (byte) 0, (byte) 255}};
    
    // Adding a player:
    private int currentPlayerNumber;  // The current player number.
    private int currentVestNumber;  // The vest numbe for a new player.
    private String currentPlayerName;  // The name of new player.
    private int currentTeam;  // The team of new player.
    private int currentHealth;  // The health of new player.
    private int currentAmmu;  // The ammunition of a new player.
    private int currentPoints;  // The points of new player.
    private boolean currentTagger;  // The tagger controller of a new player.
    
    // Adding a tag;
    private int currentTagNumber;  // The current tag number.
    private byte[] currentUID;  // The current UID.
    private String currentUIDHexString;  // The current UID as hex string.
    
    // Game play:
    private int playedGames;  // The number of played games.
    private Timer gameTimer;  // The game timer.
    private int timeInSec;  // The play time of the current game in sec.
    private boolean gamePaused;  // Indicator if the games is currently paused.
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel GameStatPanel;
    private javax.swing.JLabel ammoPerTagLabel;
    private javax.swing.JTextField ammoPerTagTextField;
    private javax.swing.JRadioButton arduinoRadioButton;
    private javax.swing.JRadioButton edisonRadioButton;
    private javax.swing.JComboBox firstTeamColorComboBox;
    private javax.swing.JLabel firstTeamColorLabel;
    private javax.swing.JLabel firstTeamNameLabel;
    private javax.swing.JTextField firstTeamNameTextField;
    private javax.swing.JLabel firstTeamScoreLabel;
    private javax.swing.JPanel firstTeamScorePanel;
    private javax.swing.JPanel firstTeamSettingsPanel;
    private javax.swing.JPanel firstTeamStatPanel;
    private javax.swing.JScrollPane fristTeamStatScrollPane;
    private javax.swing.JTable fristTeamStatTable;
    private javax.swing.JPanel gameControlPanel;
    private javax.swing.JComboBox gameModeComboBox;
    private javax.swing.JLabel gameModeInputLabel;
    private javax.swing.JLabel gameModeLabel;
    private javax.swing.JPanel gameModePanel;
    private javax.swing.JLabel gameModeTitelLabel;
    private javax.swing.JPanel gameSettingsPanel;
    private javax.swing.JLabel gameStatusInputLabel;
    private javax.swing.JLabel gameStatusLabel;
    private javax.swing.JLabel healthPerTagLabel;
    private javax.swing.JTextField healthPerTagTextField;
    private javax.swing.JTabbedPane mainTabbedPane;
    private javax.swing.JLabel maxAmmoLabel;
    private javax.swing.JTextField maxAmmoTextField;
    private javax.swing.JLabel maxHealthLabel;
    private javax.swing.JTextField maxHealthTextField;
    private javax.swing.JButton newTagButton;
    private javax.swing.JPanel overviewPanel;
    private javax.swing.JButton pauseGameButton;
    private javax.swing.JLabel playTimeMinLabel;
    private javax.swing.JLabel playedGamesInputLabel;
    private javax.swing.JLabel playedGamesTitelLabel;
    private javax.swing.JButton playerAddButton;
    private javax.swing.JLabel playerAmmoLabel;
    private javax.swing.JPanel playerAmmoPanel;
    private javax.swing.JProgressBar playerAmmoProgressBar;
    private javax.swing.JTextField playerAmmoTextField;
    private javax.swing.JButton playerCancelButton;
    private javax.swing.JButton playerDeleteButton;
    private javax.swing.JPanel playerGeneralPanel;
    private javax.swing.JPanel playerHealthPanel;
    private javax.swing.JProgressBar playerHealthProgressBar;
    private javax.swing.JLabel playerHelathLabel;
    private javax.swing.JTextField playerHelathTextField;
    private javax.swing.JPanel playerListPanel;
    private javax.swing.JPanel playerListPanel1;
    private javax.swing.JLabel playerNameLabel;
    private javax.swing.JTextField playerNameTextField;
    private javax.swing.JLabel playerNumberLabel;
    private javax.swing.JTextField playerNumberTextField;
    private javax.swing.JPanel playerPanel;
    private javax.swing.JLabel playerPointsLabel;
    private javax.swing.JPanel playerPointsPanel;
    private javax.swing.JTextField playerPointsTextField;
    private javax.swing.JPanel playerSettingsPanel;
    private javax.swing.JComboBox playerTeamComboBox;
    private javax.swing.JLabel playerTeamLabel;
    private javax.swing.JScrollPane playersListScrollPane;
    private javax.swing.JTable playersListTable;
    private javax.swing.JLabel pointsPerTagLabel;
    private javax.swing.JTextField pointsPerTagTextField;
    private javax.swing.JLabel remainingTimeInputLabel;
    private javax.swing.JLabel remainingTimeTitleLabel;
    private javax.swing.JPanel rfidPanel;
    private javax.swing.JButton saveGameSettingsButton;
    private javax.swing.JComboBox secondTeamColorComboBox;
    private javax.swing.JLabel secondTeamColorLabel;
    private javax.swing.JLabel secondTeamNameLabel;
    private javax.swing.JTextField secondTeamNameTextField;
    private javax.swing.JLabel secondTeamScoreLabel;
    private javax.swing.JPanel secondTeamScorePanel;
    private javax.swing.JPanel secondTeamSettingsPanel;
    private javax.swing.JPanel secondTeamStatPanel;
    private javax.swing.JScrollPane secondTeamStatScrollPane;
    private javax.swing.JTable secondTeamStatTable;
    private javax.swing.JPanel setGameSettingsPanel;
    private javax.swing.JLabel setPlayTimeLabel;
    private javax.swing.JPanel setPlayTimePanel;
    private javax.swing.JTextField setPlayTimeTextField;
    private javax.swing.JButton startGameButton;
    private javax.swing.JPanel statisticsPanel;
    private javax.swing.JButton stopGameButton;
    private javax.swing.JButton tagAddButton;
    private javax.swing.JCheckBox tagAddPointsCheckBox;
    private javax.swing.JTextField tagAddPointsTextField;
    private javax.swing.JButton tagCancelButton;
    private javax.swing.JButton tagDeleteButton;
    private javax.swing.JPanel tagFunctionPanel;
    private javax.swing.JPanel tagGeneralPanel;
    private javax.swing.JScrollPane tagListScrollPane;
    private javax.swing.JTable tagListTable;
    private javax.swing.JLabel tagNameLabel;
    private javax.swing.JTextField tagNameTextField;
    private javax.swing.JLabel tagNumberLabel;
    private javax.swing.JTextField tagNumberTextField;
    private javax.swing.JCheckBox tagReloadAmmoCheckBox;
    private javax.swing.JCheckBox tagReloadHealthCheckBox;
    private javax.swing.JCheckBox tagSetAmmoCheckBox;
    private javax.swing.JTextField tagSetAmmoTextField;
    private javax.swing.JCheckBox tagSetHealthCheckBox;
    private javax.swing.JTextField tagSetHelathTextField;
    private javax.swing.JPanel tagSettingsPanel;
    private javax.swing.JLabel tagUIDLabel;
    private javax.swing.JTextField tagUIDTextField;
    private javax.swing.ButtonGroup taggerTypButtonGroup;
    private javax.swing.JPanel teamSettingsPanel;
    private javax.swing.JList topPlayerList;
    private javax.swing.JPanel topPlayerPanel;
    private javax.swing.JScrollPane topPlayerScrollPane;
    private javax.swing.JLabel vestNumberLabel;
    private javax.swing.JTextField vestNumberTextField;
    // End of variables declaration//GEN-END:variables
}
