/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */

package LTServerPack;

import java.util.ArrayList;
import javax.swing.table.AbstractTableModel;

/**
 * A class representing the TabelModel for a Tabel which lists the current
 * available players.
 * 
 * @author Marc
 */
public class TableModelPlayer extends AbstractTableModel{

    // MEMBERS:
    // The column titles.
    private String[] columnNames = {"PlayerNo", "Name", "Team", "Health",
        "Ammunition", "Points", "WestNo", "Controller"};
    // The list of players.
    private ArrayList<Player> players;

    /**
     * Constructor. Sets the list of players.
     * 
     * @param p - the list of players.
     */
    public TableModelPlayer(ArrayList<Player> p) {
        this.players = p;
    }
    
    /**
     * Getter for the number of columns.
     * 
     * @return - the number of columns.
     */
    @Override
    public int getColumnCount() {
        return this.columnNames.length;
    }
    
    /**
     * Getter for the number of rows.
     * 
     * @return - the number of rows.
     */
    @Override
    public int getRowCount() {
        return players.size();
    }
    
    /**
     * Getter for a requested column title.
     * 
     * @param col - the column number.
     * @return - the title of the column.
     */
    @Override
    public String getColumnName(int col) {
        return columnNames[col];
    }
    
    /**
     * Getter for value at certain field of table.
     * 
     * @param row - the row number of the field.
     * @param col - the column number of the field.
     * @return - the value for the field.
     */
    @Override
    public Object getValueAt(int row, int col) {
        // Check if row doesn't exceed the number of list entries.
        if (row < players.size()) {
            // Check which coulmn is requested and return the corresponding
            // value.
            if (col == 0){
                return players.get(row).playerNr;
            } else if (col == 1) {
                return players.get(row).name;
            } else if (col == 2) {
                if (players.get(row).team == 0) {
                    return "Team 1";
                } 
                return "Team 2";
            } else if (col == 3) {
                return players.get(row).health;
            } else if (col == 4) {
                return players.get(row).ammu;
            } else if (col == 5) {
                return players.get(row).points;
            } else if (col == 6) {
                return players.get(row).westNr;
            } else if (col == 7) {
                if (players.get(row).edisonTagger) {
                    return "Edison";
                } else {
                    return "Arduino";
                }
            }
        }
        // If something goes wrong, return null.
        return null;
    }
    
    /**
     * Getter if a certian field of the table is editable.
     * 
     * @param row - the row number of the field.
     * @param col - the column number of the field.
     * @return if the field is editable.
     */
    @ Override
    public boolean isCellEditable(int row, int col) {
        // Set all field not editable.
        return false;
    }
}
