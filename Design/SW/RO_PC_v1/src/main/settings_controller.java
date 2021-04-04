package main;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.layout.Pane;
import serialPort.serialData;

public class settings_controller extends page_controller {
    @FXML
    private Pane rootPane;
    @FXML
    private Label moduleStatus;

    private buttonStatus disableButtonStatus;

    @Override
    public void update(serialData data) {

    }

    public enum buttonStatus {
        state1,
        state2
    };

    @FXML
    public void initialize() {
        masterRootPane = rootPane;
    }

    @FXML
    private void disableCommunicationModule() {
        switch (disableButtonStatus) {
            case state1:
                Main.getSerialPortHandler().setDisableConnection(true);
                moduleStatus.setText("Stopped");
                break;
            case state2:
                Main.getSerialPortHandler().setDisableConnection(false);
                Main.getSerialPortHandler().run(); // This line has to be checked
                moduleStatus.setText("Running");
                break;
        }

    }
}
