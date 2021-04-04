package serialPort;

import main.page_controller;
import java.io.*;
import java.net.*;
import java.util.Arrays;


public class serialPortHandler extends Thread {

    public boolean isDisableConnection() {
        return disableConnection;
    }

    public void setDisableConnection(boolean disableConnection) {
        this.disableConnection = disableConnection;
    }

    public int getReceiveSocketPortNumber() {
        return receiveSocketPortNumber;
    }

    public void setReceiveSocketPortNumber(int receiveSocketPortNumber) {
        this.receiveSocketPortNumber = receiveSocketPortNumber;
    }

    private boolean disableConnection;
    private int receiveSocketPortNumber,sendSocketPortNumber;
    public page_controller getObserver() {
        return observer;
    }
    public void setObserver(page_controller observer) {
        this.observer = observer;
    }
    private page_controller observer;
    public serialPortHandler() {


        dataPacket = new serialData();
        disableConnection = false;
        receiveSocketPortNumber = 27015;
        sendSocketPortNumber = 27016;

        try {
            ReceiveSocket = new DatagramSocket(receiveSocketPortNumber);
        } catch (SocketException e) {
            e.printStackTrace();
        }
        System.out.println("Server is listening on port " + receiveSocketPortNumber);

        try {
            SendSocket = new DatagramSocket(sendSocketPortNumber);
        } catch (SocketException e) {
            e.printStackTrace();
        }
        System.out.println("Server is sending data from " + sendSocketPortNumber);

        // Send socket details
        send_address = null;
        send_port = 0;

        //Establish connection with sending socket
        System.out.println("Waiting for establishing connection with the communication module...");
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        try {
            SendSocket.receive(packet);
            String received = new String(packet.getData());
            System.out.println(received);
            send_address = packet.getAddress();
            send_port = packet.getPort();
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("Connection established");


    }
    serialData dataPacket;
    private DatagramSocket ReceiveSocket,SendSocket;
    private boolean running;
    private byte[] buf = new byte[256];
    private enum stateMachine {
        IDLE,
        BOARD,
        CHAMBER,
        HEAT_PROFILE
    }
    private InetAddress send_address;
    private int send_port;

    public void writeSocket(byte[] byte_array) {
        //System.out.println("Write operation started.");
        DatagramPacket out_packet = new DatagramPacket(byte_array, byte_array.length, send_address, send_port);

        try {
            SendSocket.send(out_packet);
        } catch (IOException e) {
            e.printStackTrace();
        }
        //System.out.println("Write operation completed.");
    }

    public void run() {
        stateMachine rxStateMachine = stateMachine.IDLE;
        running = true;

        while (running) {

            DatagramPacket packet = new DatagramPacket(buf, buf.length);

            try {
                ReceiveSocket.receive(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }

            //InetAddress address = packet.getAddress();
            //int port = packet.getPort();
            packet = new DatagramPacket(buf, buf.length, send_address, send_port);

            String received = new String(packet.getData());
            //System.out.println(received);

            switch (rxStateMachine) {
                case IDLE:
                    if (buf[0]==98)
                        rxStateMachine=stateMachine.BOARD;
                    else if (buf[0]==99)
                        rxStateMachine=stateMachine.CHAMBER;
                    break;
                case BOARD:
                    dataPacket.setBoardTemp(Double.parseDouble(received));
                    rxStateMachine = stateMachine.IDLE;
                    break;
                case CHAMBER:
                    dataPacket.setChamberTemp(Double.parseDouble(received));
                    rxStateMachine = stateMachine.IDLE;
                    break;
                case HEAT_PROFILE:
                    rxStateMachine = stateMachine.IDLE;
                    break;

            }

            if (observer!=null)
                observer.update(dataPacket);


            Arrays.fill(buf,(byte)0);



        }
        ReceiveSocket.close();
    }
}