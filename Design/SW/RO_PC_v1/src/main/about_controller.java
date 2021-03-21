package main;

import javafx.fxml.FXML;
import javafx.scene.layout.Pane;

public class about_controller extends page_controller{
    @FXML
    private Pane rootPane;

    @FXML
    public void initialize() {
        masterRootPane = rootPane;
    }
}
