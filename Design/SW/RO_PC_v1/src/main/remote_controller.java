package main;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

import javafx.beans.Observable;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.stage.FileChooser;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.XYChart;
import javafx.scene.control.TextField;
import javafx.scene.layout.Pane;
import javafx.stage.FileChooser;

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
    private Label fileNameLabel;
    @FXML
    private Label numberOfRecordsLabel;

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
}
