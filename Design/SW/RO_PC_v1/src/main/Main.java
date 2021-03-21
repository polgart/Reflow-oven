package main;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception{

        /* Load resources*/
        FXMLLoader mainLoader = new FXMLLoader(getClass().getResource("window_frame.fxml"));
        Parent root = mainLoader.load();

        FXMLLoader aboutLoader = new FXMLLoader(getClass().getResource("about_page.fxml"));
        Parent about = aboutLoader.load();

        FXMLLoader settingsLoader = new FXMLLoader(getClass().getResource("settings_page.fxml"));
        Parent settings = settingsLoader.load();


        /* Get controllers */
        window_frame_controller mainController = mainLoader.getController();
        about_controller aboutController = aboutLoader.getController();
        settings_controller settingsController = settingsLoader.getController();


        /* Create scenes */
        Scene mainScene = new Scene(root);
        Scene aboutScene = new Scene(about);
        Scene settingsScene = new Scene(settings);


        /* Initialize controllers */
        mainController.setAboutPage(aboutScene);
        mainController.setSettingsPage(settingsScene);

        /* Bind to frame width*/
        settingsController.bindRootPane(mainController.getMainPane());
        aboutController.bindRootPane(mainController.getMainPane());

        /* Show stage */
        primaryStage.setTitle("Reflow oven controller");
        primaryStage.setScene(mainScene);
        primaryStage.show();

        /* Set start page */
        mainController.loadStartPage();

    }

    public static void main(String[] args) {
        launch(args);
    }
}
