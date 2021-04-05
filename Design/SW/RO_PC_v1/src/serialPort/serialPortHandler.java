package serialPort;

import main.page_controller;
import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
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
    private byte[] buf = new byte[255];
    private int receiveDataCounter;
    private enum stateMachine {
        IDLE,
        BOARD,
        CHAMBER,
        HEAT_PROFILE,
        TIME,
        ID,
        NAME,
        HEAT_PROFILE_LENGTH,
        HEAT_PROFILE_TIME,
        HEAT_PROFILE_DATA
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

            String received = new String(packet.getData(), StandardCharsets.UTF_8);
            //System.out.println(received);


            switch (rxStateMachine) {
                case IDLE:
                    if (buf[0]==98)
                        rxStateMachine=stateMachine.BOARD;
                    else if (buf[0]==99)
                        rxStateMachine=stateMachine.CHAMBER;
                    else if (buf[0]==100)
                        rxStateMachine=stateMachine.TIME;
                    else if (buf[0]==101)
                        rxStateMachine=stateMachine.NAME;
                    else if (buf[0]==102)
                        rxStateMachine=stateMachine.ID;
                    else if (buf[0]==103)
                        rxStateMachine=stateMachine.HEAT_PROFILE_LENGTH;
                    else if (buf[0]==104) {
                         rxStateMachine = stateMachine.HEAT_PROFILE_DATA;
                        this.receiveDataCounter=0;
                        dataPacket.init_heat_profile_temperature();
                    }
                    else if (buf[0]==105) {
                        rxStateMachine=stateMachine.HEAT_PROFILE_TIME;
                        this.receiveDataCounter=0;
                        dataPacket.init_heat_profile_time();
                    }
                    else
                        System.out.println("Unknown packet arrived:"+received);
                    break;
                case BOARD:
                    dataPacket.setType(serialData.dataTypeEnum.ONLY_TEMPERATURE);
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
                case TIME:
                    dataPacket.setTime(Double.parseDouble(received)/100.0);
                    dataPacket.setType(serialData.dataTypeEnum.SERIAL_DATA);
                    rxStateMachine = stateMachine.IDLE;
                    break;
                case NAME:
                    dataPacket.setFinishedReceiving(false);
                    //System.out.println("Profile name: "+received);
                    dataPacket.setType(serialData.dataTypeEnum.HEAT_PROFILE);
                    dataPacket.setName(received);
                    rxStateMachine = stateMachine.IDLE;
                    break;
                case ID:
                    //System.out.println("Profile ID: "+received);
                    dataPacket.setID((int)Double.parseDouble(received));
                    rxStateMachine = stateMachine.IDLE;
                    break;
                case HEAT_PROFILE_DATA:
                    //System.out.println("Profile temperature: "+received);
                    dataPacket.add_heat_profile_temperature(Double.parseDouble(received)/4,this.receiveDataCounter);
                    this.receiveDataCounter++;
                    if (this.receiveDataCounter == dataPacket.getLength()+1)
                        rxStateMachine = stateMachine.IDLE;
                    break;
                case HEAT_PROFILE_TIME:
                    //System.out.println("Profile time: "+received);
                    dataPacket.add_heat_profile_time(Double.parseDouble(received)/10,this.receiveDataCounter);
                    this.receiveDataCounter++;
                    if (this.receiveDataCounter == dataPacket.getLength()+1) {
                        System.out.println("Data packet is complete");
                        dataPacket.setFinishedReceiving(true);
                        rxStateMachine = stateMachine.IDLE;
                    }
                    break;
                case HEAT_PROFILE_LENGTH:
                    //System.out.println("Profile length: "+received);
                    dataPacket.setLength((int)Double.parseDouble(received));
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