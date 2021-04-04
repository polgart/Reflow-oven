package main;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;
import serialPort.serialData;

public abstract class page_controller {

    protected Pane masterRootPane;

    public void bindRootPane(Pane mainPane) {
        ((Pane)masterRootPane.getChildren().get(0)).prefWidthProperty().bind(mainPane.widthProperty());
    }

    abstract public void update(serialData data);
}
