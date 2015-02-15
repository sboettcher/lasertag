/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
           Benjamin Völker <voelkerb@tf.uni-freiburg.de>
 */

package LTServerPack;

import javax.swing.SwingUtilities;
import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import jssc.SerialPortList;

/**
 * A class which the connection to a serial port.
 * 
 * @author Marc
 */
public class SerialHandler {
    
    /**
     * Contrustor. Sets the main GUI.
     * 
     * @param lts - the main GUI.
     */
    public SerialHandler(LTServer lts) {
        ltServer = lts;
    }
    
    /**
     * Returns a List of all available serial ports.
     * 
     * @return - List of all available ports.
     */
    public String[] getSerialPorts() {
        return SerialPortList.getPortNames();
    }
    
    /**
     * Connects to given serial port.
     * 
     * @param portName - the name of the port.
     * @param baudRate - the baudrate for the connection.
     * @return  - if connection was sucessful.
     */
    public boolean connect(String portName, int baudRate) {
        // Create the serial port.
        serialPort = new SerialPort(portName);
        try {
            serialPort.openPort(); // Open the port.
            serialPort.setParams(baudRate, 8, 1, 0); // Set connection params.
            int mask = SerialPort.MASK_RXCHAR; // Prepare mask.
            serialPort.setEventsMask(mask); // Set mask.
            // Create a reader for the port.
            SerialPortReader serialPortReader = new SerialPortReader();
            //Add SerialPortEventListener.
            serialPort.addEventListener(serialPortReader);
        } catch (SerialPortException ex) {
            // If somethings goes wrong signalize that.
            System.err.println(ex);
            return false;
        }
        // If everythings succeeds, return true.
        return true;
    }
    
    /**
     * A class which handles the incoming data on an serial port.
     */
    static class SerialPortReader implements SerialPortEventListener {
        
        /**
         * Method which is called, if there something going on at a serial
         * connection. If there is new serial data, it is passed to the main
         * GUI.
         * 
         * @param event - the event on the serial connection.
         */
        @Override
        public void serialEvent(SerialPortEvent event) {
            if (event.isRXCHAR()&& event.getEventValue() > 0) {
                // If serial data is available, read it and schedule the
                // processing of it in the main GUI thread.
                try {
                    final byte[] in = serialPort.readBytes();
                    SwingUtilities.invokeLater(new Runnable() {
                        @Override
                        public void run()
                        {
                            ltServer.newRFIDData(in);
                        }
                    });
                } catch (SerialPortException ex) {
                    System.err.println(ex);
                }

            }
        }
    }
    
    // MEMBERS:
    // The current serial port.
    static SerialPort serialPort;
    // The main GUI.
    static LTServer ltServer;
}
