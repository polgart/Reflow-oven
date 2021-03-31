package serialPort;

import java.util.Observable;

public class serialData extends Observable {
    double chamberTemp;

    public double getChamberTemp() {
        return chamberTemp;
    }

    public double getBoardTemp() {
        return boardTemp;
    }

    public Integer getTimestamp() {
        return timestamp;
    }

    public boolean isScv() {
        return scv;
    }

    public boolean isScg() {
        return scg;
    }

    public boolean isOc() {
        return oc;
    }

    double boardTemp;
    Integer timestamp;
    boolean scv;
    boolean scg;
    boolean oc;

    void serialData(double chamberTemp_sD, double boardTemp_sD, Integer timestamp_sD, boolean scv_sD, boolean scg_sD, boolean oc_sD) {
        chamberTemp=chamberTemp_sD;
        boardTemp=boardTemp_sD;
        timestamp=timestamp_sD;
        scv=scv_sD;
        scg=scg_sD;
        oc=oc_sD;
    }
}
