/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 */


package LTServerPack;

import java.util.ArrayList;
import javax.swing.table.AbstractTableModel;

/**
 * A class representing the TabelModel for a Tabel which lists all currently
 * available RFID tags.
 * 
 * @author Marc
 */
public class TableModelTags extends AbstractTableModel{
    
    // MEMBERS:
    // The column titles.
    private final String[] columnNames = {"TagNo", "UID", "Info/Name", "ReloadAmmu", 
        "ReloadHealth", "SetAmmuTo", "SetHealthTo", "SetPointsTo"};
    // The list of tags.
    private ArrayList<Tag> tags;
    
    /**
     * Constructor. Sets the list of tags.
     * 
     * @param t - the list of tags.
     */
    public TableModelTags(ArrayList<Tag> t) {
        this.tags = t;
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
        return tags.size();
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
        if (row < tags.size()) {
            // Check which coulmn is requested and return the corresponding
            // value.
            if (col == 0){
                return tags.get(row).tagNo;
            } else if (col == 1) {
                return tags.get(row).uidHexString;
            }else if (col == 2) {
                return tags.get(row).name;
            } else if (col == 3) {
                return tags.get(row).reloadAmmu;
            } else if (col == 4) {
                return tags.get(row).reloadHealth;
            } else if (col == 5) {
                if (tags.get(row).ammuTo) {
                    return tags.get(row).ammuToValue;
                } else {
                    return "false";
                }
            } else if (col == 6) {
                if (tags.get(row).healthTo) {
                    return tags.get(row).healthToValue;
                } else {
                    return "false";
                }
            } else if (col == 7) {
                if (tags.get(row).pointsTo) {
                    return tags.get(row).pointsToValue;
                } else {
                    return "false";
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
    @Override
    public boolean isCellEditable(int row, int col) {
        // Set all field not editable.
        return false;
    }
}