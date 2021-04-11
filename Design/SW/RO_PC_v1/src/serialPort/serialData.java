package serialPort;

import java.util.Arrays;


public class serialData {

    public enum dataTypeEnum {
        SERIAL_DATA_WITH_DESIRED_TEMP,
        SERIAL_DATA,
        ONLY_TEMPERATURE,
        HEAT_PROFILE
    };

    public serialData() {
        heat_profile_temperature = new Double[512];
        heat_profile_time = new Double[512];
    }

    public double getChamberTemp() {
        return chamberTemp;
    }

    public double getBoardTemp() {
        return boardTemp;
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

    public void setScv(boolean scv) {
        this.scv = scv;
    }

    public void setScg(boolean scg) {
        this.scg = scg;
    }

    public void setOc(boolean oc) {
        this.oc = oc;
    }


    public double getTime() {
        return time;
    }

    public void setTime(double time) {
        this.time = time;
    }


    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getID() {
        return ID;
    }

    public void setID(int ID) {
        this.ID = ID;
    }

    public int getLength() {
        return length;
    }

    public void setLength(int length) {
        this.length = length;
    }

    public void add_heat_profile_time(Double data, int index) {
        this.heat_profile_time[index] = data;
    }

    public void add_heat_profile_temperature(Double data, int index) {
        this.heat_profile_temperature[index] = data;
    }

    public void init_heat_profile_time() {
        Arrays.fill(heat_profile_time, 0.0);
    }

    public void init_heat_profile_temperature() {
        Arrays.fill(heat_profile_temperature, 0.0);
    }

    public Double[] get_heat_profile_time() {
        return heat_profile_time;
    }

    public Double[] get_heat_profile_temperature() {
        return heat_profile_temperature;
    }

    public Double get_heat_profile_time_by_index(int index) {
        return heat_profile_time[index];
    }

    public Double get_heat_profile_temperature_by_index(int index) {
        return heat_profile_temperature[index];
    }

    public double getDesiredTemp() {
        return desiredTemp;
    }

    public void setDesiredTemp(double desiredTemp) {
        this.desiredTemp = desiredTemp;
    }

    double time;
    double chamberTemp;
    double boardTemp;
    double desiredTemp;
    boolean scv;
    boolean scg;
    boolean oc;
    String name;
    int ID;
    int length;
    Double[] heat_profile_time;
    Double[] heat_profile_temperature;

    public boolean isFinishedReceiving() {
        return finishedReceiving;
    }

    public void setFinishedReceiving(boolean finishedReceiving) {
        this.finishedReceiving = finishedReceiving;
    }

    boolean finishedReceiving;

    public dataTypeEnum getType() {
        return type;
    }

    public void setType(dataTypeEnum type) {
        this.type = type;
    }

    private dataTypeEnum type;

}
