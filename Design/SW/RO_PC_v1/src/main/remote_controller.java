package main;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.lang.Runnable;

import javafx.application.Platform;
import javafx.beans.Observable;
import javafx.fxml.FXML;
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
    private TextField xData;

    @FXML
    private TextField yData;

    @FXML
    private Label tChamber;

    @FXML
    private Label tBoard;


    @FXML
    private Label fileNameLabel;
    @FXML
    private Label numberOfRecordsLabel;
    @FXML
    private Label statusLabel;

    @FXML
    private void loadCSV() {
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
                while ((line = br.readLine()) != null) {
                    String[] values = line.split(";");
                    Double xVal = Double.parseDouble(values[0]);
                    Double yVal = Double.parseDouble(values[1]);
                    csv_series.getData().add(new XYChart.Data(xVal,yVal));
                }
                Integer numberOfLines = (csv_series.getData().size());
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

    @FXML
    private void addDummyData() {

        if (xData.getCharacters().length()==0 || yData.getCharacters().length()==0) {
            return;
        }
        Double xVal = Double.parseDouble(xData.getCharacters().toString());
        Double yVal = Double.parseDouble(yData.getCharacters().toString());
        series.getData().add(new XYChart.Data(xVal,yVal));

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
        byte[] dataOut = {10, 1, 0};
        Main.getSerialPortHandler().writeSocket(dataOut);
        series.getData().clear();
        statusLabel.setText("Running");
        statusLabel.setStyle("-fx-background-color: #7ee868;");
    }

    @FXML
    private void stopBtnCallback() {
        byte[] dataOut = {10, 2, 0};
        Main.getSerialPortHandler().writeSocket(dataOut);
        statusLabel.setText("Stopped");
        statusLabel.setStyle("-fx-background-color: #FF1919;");
    }

    @FXML
    private void loadBtnCallback() {
        byte[] dataOut = {10, 3, 0};
        Main.getSerialPortHandler().writeSocket(dataOut);
        statusLabel.setText("Reading");
        statusLabel.setStyle("-fx-background-color: #f0e767;");
    }

    public remote_controller() {
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
                        statusLabel.setText("Loaded");
                    }
                    break;
            }

        });
    }
}
