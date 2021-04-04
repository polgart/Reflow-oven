package serialPort;

public class serialData {


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

    public void setChamberTemp(double chamberTemp) {
        this.chamberTemp = chamberTemp;
    }

    public void setBoardTemp(double boardTemp) {
        this.boardTemp = boardTemp;
    }

    public void setTimestamp(Integer timestamp) {
        this.timestamp = timestamp;
    }

    public void setScv(boolean scv) {
        this.scv = scv;
    }

    public void setScg(boolean scg) {
        this.scg = scg;
    }

    public void setOc(boolean oc) {
        this.oc = oc;
    }



    double chamberTemp;
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
