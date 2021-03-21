package main;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;

public class page_controller {

    protected Pane masterRootPane;

    public void bindRootPane(Pane mainPane) {
        ((Pane)masterRootPane.getChildren().get(0)).prefWidthProperty().bind(mainPane.widthProperty());
    }
}
