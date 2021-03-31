package serialPort;
import java.io.*;
import java.net.*;

/**
 * This program demonstrates a simple TCP/IP socket server that echoes every
 * message from the client in reversed form.
 * This server is single-threaded.
 *
 * @author www.codejava.net
 */
public class serialPortHandler extends Thread {

    public boolean isDisableConnection() {
        return disableConnection;
    }

    public void setDisableConnection(boolean disableConnection) {
        this.disableConnection = disableConnection;
    }

    public int getSocketPortNumber() {
        return socketPortNumber;
    }

    public void setSocketPortNumber(int socketPortNumber) {
        this.socketPortNumber = socketPortNumber;
    }

    private boolean disableConnection;
    private int socketPortNumber;



    public serialPortHandler() {
        disableConnection = false;
        socketPortNumber = 27015;
    }


    public void run() {

        int port = socketPortNumber;

        try (ServerSocket serverSocket = new ServerSocket(port)) {

            System.out.println("Server is listening on port " + port);


                Socket socket = serverSocket.accept();
                System.out.println("New client connected");

                InputStream input = socket.getInputStream();
                BufferedReader reader = new BufferedReader(new InputStreamReader(input));

                OutputStream output = socket.getOutputStream();
                PrintWriter writer = new PrintWriter(output, true);


                String text;

                do {
                    text = reader.readLine();
                    if (text != null) {
                        System.out.println(text);
                        String reverseText = new StringBuilder(text).reverse().toString();
                        writer.println("Server: " + reverseText + "end .. \0");
                    }
                } while (!disableConnection);
                System.out.println("Socket closed");
                socket.close();


        } catch (IOException ex) {
            System.out.println("Exception");
            System.out.println("Server exception: " + ex.getMessage());
            ex.printStackTrace();
        }
    }
}