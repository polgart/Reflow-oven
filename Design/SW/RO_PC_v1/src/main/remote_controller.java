package main;

import java.io.*;
import java.util.*;
import java.lang.Runnable;

import javafx.application.Platform;
import javafx.beans.Observable;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.layout.Background;
import javafx.stage.FileChooser;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.XYChart;
import javafx.scene.control.TextField;
import javafx.scene.layout.Pane;
import javafx.stage.FileChooser;
import serialPort.serialData;

public class remote_controller extends page_controller {

    private XYChart.Series series;
    private XYChart.Series csv_series;

    @FXML
    private Pane rootPane;

    @FXML
    private LineChart remoteChart;

    @FXML
    private Label tChamber;

    @FXML
    private Label tBoard;

    @FXML
    private Label heatProfileName;

    @FXML
    private Label fileNameLabel;
    @FXML
    private Label numberOfRecordsLabel;
    @FXML
    private Label statusLabel;

    @FXML
    private TextField hp_generate_name;

    @FXML
    private TextField hp_generate_id;

    @FXML
    private CheckBox interpolateCheckBox;

    @FXML
    private void loadCSV() {
        this.clearRemoteChartData();
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Resource File");
        fileChooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("Comma separated files", "*.csv")
        );
        File selectedFile = fileChooser.showOpenDialog(null);
        if (selectedFile != null) {
            fileNameLabel.setText(selectedFile.getName());
            try (BufferedReader br = new BufferedReader(new FileReader(selectedFile.getAbsolutePath()))) {
                String line;
                int it = 0;
                while ((line = br.readLine()) != null) {
                    String[] values = line.split(";");
                    Double xVal = Double.parseDouble(values[0]);
                    Double yVal = Double.parseDouble(values[1]);
                    this.CSV_time[it] = xVal;
                    this.CSV_temperature[it] = yVal;
                    csv_series.getData().add(new XYChart.Data(xVal,yVal));
                    it++;
                }
                Integer numberOfLines = (csv_series.getData().size());
                this.CSV_records = numberOfLines;
                numberOfRecordsLabel.setText(numberOfLines.toString());
            } catch (IOException ex) {
                numberOfRecordsLabel.setText(ex.toString());
            }

        }

    }
    @FXML
    public void initialize() {

        masterRootPane = rootPane;
        series = new XYChart.Series();
        csv_series = new XYChart.Series();
        csv_series.setName("Heat profile");
        series.setName("Desired temperature");
        remoteChart.getData().add(series);
        remoteChart.getData().add(csv_series);
        remoteChart.setCreateSymbols(false); // Set false for disable markers
    }

    private byte uByte(int data) {
        if (0 <= data && data <= 127)
            return  (byte)data;
        if (128 <= data && data <= 255)
            return (byte)(data - 256);
        return (byte)0;
    }
    private byte uByte(char data) {
        if (0 <= data && data <= 127)
            return  (byte)data;
        if (128 <= data && data <= 255)
            return (byte)(data - 256);
        return (byte)0;
    }
    private Double[] CSV_temperature;
    private Double[] CSV_time;
    private int CSV_records;

    @FXML
    private void createCommandByteArrayFromCSV() {
        if (csv_series.getData().size() == 0)
            this.loadCSV();

        int seriesLength = this.CSV_records-1;

        byte[] hpn = this.hp_generate_name.getText().getBytes();
        byte hpid = (byte)Integer.parseUnsignedInt(this.hp_generate_id.getText());

        Vector<Byte> commandVector = new Vector<>();
        commandVector.add(uByte(1));
        for (int i = 0; i < 8; i++) {
            if (i < hpn.length)
                commandVector.add(hpn[i]);
            else
                commandVector.add((byte)0);
        }

        commandVector.add(hpid);
        commandVector.add(uByte(seriesLength % 256));
        commandVector.add(uByte(seriesLength / 256));


        for (int i = 0; i <= seriesLength; i++) {
            commandVector.add(uByte((int)(this.CSV_temperature[i]*4 % 256)));
            commandVector.add(uByte((int)(this.CSV_temperature[i]*4 / 256)));
        }

        for (int i = 0; i <= seriesLength; i++) {
            commandVector.add(uByte((int)(this.CSV_time[i]*10 % 256)));
            commandVector.add(uByte((int)(this.CSV_time[i]*10 / 256)));
        }

        byte[] outputCommand = new byte[commandVector.size()];
        int p=0;
        for(Byte b : commandVector)
            outputCommand[p++] = b;
        Main.getSerialPortHandler().writeSocket(outputCommand);

        byte[] dataOut2 = {10, 4}; // Load current profile from buffer
        Main.getSerialPortHandler().writeSocket(dataOut2);

        this.statusLabel.setText("Generated");
        statusLabel.setStyle("-fx-background-color: #f0e767;");
    }

    @FXML
    private void loadDefaultHeatProfile() {
        byte[] dataOut = {10, 5};
        Main.getSerialPortHandler().writeSocket(dataOut);

        this.statusLabel.setText("Default");
        statusLabel.setStyle("-fx-background-color: #f0e767;");
    }

    @FXML
    private void addDummyData() {
        // name:Test ID:45 length:2 data:600 (150C) 960 (240C) 600 (150C) time: 0(0s) 1550(155s) 2350(235s)
        byte[] dataOut = {1, 'T', 'e', 's', 't', 0, 0, 0, 0, 42, 2, 0, 88, 2, uByte(192), 3, 88, 2, 0, 0, 14, 6, 46, 9};
        Main.getSerialPortHandler().writeSocket(dataOut);

        byte[] dataOut2 = {10, 4}; // Load current profile from buffer
        Main.getSerialPortHandler().writeSocket(dataOut2);
    }

    @FXML
    private void addDataToSeries(Double xVal,Double yVal) {
        series.getData().add(new XYChart.Data(xVal,yVal));
    }

    @FXML
    private void addDataToRemoteChart(Double xVal,Double yVal) {
        csv_series.getData().add(new XYChart.Data(xVal,yVal));
    }

    @FXML
    private void clearRemoteChartData() {
        csv_series.getData().clear();
    }

    @FXML
    private void startBtnCallback() {
        byte[] dataOut = {10, 1};
        Main.getSerialPortHandler().writeSocket(dataOut);
        series.getData().clear();
        if (this.interpolateCheckBox.isSelected() == true ) {
            dataOut[1] = 6;
            Main.getSerialPortHandler().writeSocket(dataOut);
            csv_series.getData().clear();
        }
        statusLabel.setText("Running");
        statusLabel.setStyle("-fx-background-color: #7ee868;");
    }

    @FXML
    private void stopBtnCallback() {
        byte[] dataOut = {10, 2};
        Main.getSerialPortHandler().writeSocket(dataOut);
        statusLabel.setText("Stopped");
        statusLabel.setStyle("-fx-background-color: #FF1919;");
    }

    @FXML
    private void loadBtnCallback() {
        byte[] dataOut = {10, 3};
        Main.getSerialPortHandler().writeSocket(dataOut);
        statusLabel.setText("Reading");
        statusLabel.setStyle("-fx-background-color: #f0e767;");
    }

    public remote_controller() {
        CSV_temperature = new Double[512];
        CSV_time = new Double[512];
        CSV_records = 0;
        Main.getSerialPortHandler().setObserver(this);
    }

    @Override
    public void update(serialData data) {
        Platform.runLater(() -> {
            String boardTemp,chamberTemp;
            switch (data.getType()) {
                case ONLY_TEMPERATURE:
                    //System.out.println("Remote controller received ONLY_TEMPERATURE data");
                    boardTemp = String.valueOf(data.getBoardTemp());
                    chamberTemp = String.valueOf(data.getChamberTemp());
                    tBoard.setText(boardTemp);
                    tChamber.setText(chamberTemp);
                    break;
                case SERIAL_DATA:
                    //System.out.println("Remote controller received SERIAL_DATA data");
                    boardTemp = String.valueOf(data.getBoardTemp());
                    chamberTemp = String.valueOf(data.getChamberTemp());
                    tBoard.setText(boardTemp);
                    tChamber.setText(chamberTemp);
                    addDataToSeries(data.getTime(),data.getChamberTemp());
                    break;
                case HEAT_PROFILE:
                    if (data.isFinishedReceiving()) {
                        //System.out.println("Remote controller received HEAT_PROFILE data");
                        Double[] local_heat_profile_time_copy = data.get_heat_profile_time().clone();
                        Double[] local_heat_profile_temperature_copy = data.get_heat_profile_temperature().clone();
                        this.clearRemoteChartData();
                        for (int i = 0; i <= data.getLength(); i++) {
                            this.addDataToRemoteChart(local_heat_profile_time_copy[i], local_heat_profile_temperature_copy[i]);
                        }
                        heatProfileName.setText(data.getName());
                        statusLabel.setText("Loaded");
                    }
                    break;
                case SERIAL_DATA_WITH_DESIRED_TEMP:
                    //System.out.println("Remote controller received SERIAL_DATA data");
                    boardTemp = String.valueOf(data.getBoardTemp());
                    chamberTemp = String.valueOf(data.getChamberTemp());
                    tBoard.setText(boardTemp);
                    tChamber.setText(chamberTemp);
                    addDataToSeries(data.getTime(),data.getChamberTemp());
                    addDataToRemoteChart(data.getTime(),data.getDesiredTemp());
                    break;
            }

        });
    }
}
