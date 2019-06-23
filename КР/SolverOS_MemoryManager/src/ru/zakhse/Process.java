package ru.zakhse;

/**
 * Created by Zakhse on 20.03.17.
 */
public class Process {
    public enum State {NON_STARTED, WAITING, READY, RUNNING, FINISHED}

    public Process(int index, int startTime, int cpuBurst, int memory) {
        this.index = index;
        this.cpuBurst = cpuBurst;
        this.startTime = startTime;
        this.memory = memory;
    }

    public int index;
    public int startTime;
    public int cpuBurst;
    public int memory;
    private State state = State.NON_STARTED;

    public State getState() {return state;}

    public String getStringState() {
        switch (state) {
            case NON_STARTED:
                return "_";
            case WAITING:
                return "О";
            case READY:
                return "Г";
            case RUNNING:
                return "И";
            case FINISHED:
                return "_";
        }
        return "";
    }

    public void setState(State state) {this.state = state;}

    @Override
    public Process clone() {
        return new Process(index, startTime, cpuBurst, memory);
    }
}
