package main;

import javafx.beans.property.ReadOnlyDoubleProperty;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.Pane;

public class window_frame_controller {

    public window_frame_controller() {
        bufferedScene=null;
    }

    @FXML
    private Pane mainPane;

    @FXML
    private void loadSettingsPage() {
        loadPage(settingsPage);
    }

    @FXML
    private void loadAboutPage() {
        loadPage(aboutPage);
    }

    @FXML
    private void loadRemotePage() { loadPage(remotePage); }

    Scene bufferedScene;

    private void loadPage(Scene page) {

        ObservableList<Node> windowContent = mainPane.getChildren();
        if (bufferedScene != null) {
            Pane bufferedPane = (Pane) bufferedScene.getRoot();
            bufferedPane.getChildren().addAll(windowContent);
        }

        ObservableList<Node> newContent = page.getRoot().getChildrenUnmodifiable();
        mainPane.getChildren().addAll(newContent);
        bufferedScene = page;

    }

    private Scene aboutPage;
    private Scene settingsPage;
    private Scene remotePage;


    /* Initial page */
    public void loadStartPage() {
        loadAboutPage();
    }

    /* External FXML scenes */
    public void setAboutPage(Scene page) { aboutPage = page; }
    public void setSettingsPage(Scene page) {
        settingsPage = page;
    }
    public void setRemotePage(Scene page) {remotePage = page; }

    /* Getters */
    public Pane getMainPane() {
        return mainPane;
    }


}
