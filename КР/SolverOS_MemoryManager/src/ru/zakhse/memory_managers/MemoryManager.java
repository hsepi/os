package ru.zakhse.memory_managers;

import ru.zakhse.Process;

import java.util.Comparator;
import java.util.List;
import java.util.Objects;
import java.util.PriorityQueue;

public abstract class MemoryManager {
    // RR - Round Robin
    // FCFS - First Come First Served
    // SJF - Shortest Job First (non-preemptive)
    // SJFPreemptive - Shortest Job First (preemptive)
    public enum ShedullingPolicy {
        RR, FCFS, SJF, SJFPreemptive
    }

    public enum MemoryAllocation {bestFit, worstFit, firstFit}

    // each element of array is 10 MB
    protected Process[] allocatedMemory;

    public MemoryManager(int memorySize, int quantum, ShedullingPolicy policy, MemoryAllocation allocation,
                         Process[] processes) {
        this.memorySize = memorySize;
        this.policy = policy;
        this.allocation = allocation;
        this.quantum = quantum;
        this.processes = new Process[processes.length];
        for (int i = 0; i < processes.length; i++) {
            Process tmp = processes[i];
            this.processes[i] = tmp.clone();
        }

        table = new String[processes.length + 1][];
        for (int i = 0; i < processes.length + 1; i++) {
            table[i] = new String[50];
        }
        table[0][0] = "_";
        for (int i = 1; i <= processes.length; i++)
            table[i][0] = String.valueOf(i);

        allocatedMemory = new Process[memorySize];
    }

    protected int memorySize;
    protected ShedullingPolicy policy;
    protected MemoryAllocation allocation;
    protected int quantum;
    protected Process[] processes;

    public String getName() {
        StringBuilder str = new StringBuilder();
        switch (policy) {
            case RR:
                str.append("RR (Round Robin)");
                break;
            case FCFS:
                str.append("FCFS (First Come First Served)");
                break;
            case SJF:
                str.append("SJF (Shortest Job First non-preemptive)");
                break;
            case SJFPreemptive:
                str.append("SJFPreemptive (Shortest Job First preemptive)");
                break;
        }
        str.append(" ");
        switch (allocation) {
            case bestFit:
                str.append("best-fit");
                break;
            case worstFit:
                str.append("worst-fit");
                break;
            case firstFit:
                str.append("first-fit");
                break;
        }
        return str.toString();
    }

    protected String[][] table;


    public void solve() {}

    protected boolean isAllProcessFinished(Process[] processes) {
        for (int i = 0; i < processes.length; i++)
            if (processes[i].cpuBurst > 0)
                return false;
        return true;
    }

    protected boolean pushProcess(Process[] memory, Process process, MemoryAllocation allocation) {

        int index = -1;
        if (allocation == MemoryAllocation.bestFit) {
            int min = memory.length + 1;
            int currSize = 0;
            for (int i = 0; i < memory.length; i++) {
                if (memory[i] == null) {
                    currSize++;
                } else {
                    if (currSize < min && currSize >= process.memory) {
                        min = currSize;
                        index = i - currSize;
                    }
                    currSize = 0;
                }
            }

            if (currSize < min && currSize >= process.memory) {
                min = currSize;
                index = memory.length - currSize;
            }
        } else if (allocation == MemoryAllocation.worstFit) {
            int max = 0;
            int currSize = 0;
            for (int i = 0; i < memory.length; i++) {
                if (memory[i] == null) {
                    currSize++;
                } else {
                    if (currSize > max && currSize >= process.memory) {
                        max = currSize;
                        index = i - currSize;
                    }
                    currSize = 0;
                }
            }
            if (currSize > max && currSize >= process.memory) {
                max = currSize;
                index = memory.length - currSize;
            }
        } else {
            int currSize = 0;
            for (int i = 0; i < memory.length; i++) {
                if (memory[i] == null) {
                    currSize++;
                    if (currSize == process.memory) {
                        index = i + 1 - currSize;
                        break;
                    }
                } else {
                    currSize = 0;
                }
            }
        }

        if (index != -1) {
            for (int i = 0; i < process.memory; i++)
                memory[index + i] = process;
            return true;
        } else return false;
    }

    @Override
    public String toString() {
        int allLettersCounter = 0;
        int waitingCount = 0;
        StringBuilder str = new StringBuilder();
        str.append(getName());
        str.append("\n");
        for (int i = 0; i < table.length; i++) {
            for (int j = 0; j < table[i].length; j++) {
                if (table[i][j] == null) {
                    str.append("\n");
                    break;
                }
                String letter = table[i][j];

                if (Objects.equals(letter, "И")) {
                    allLettersCounter++;
                } else if (Objects.equals(letter, "Г") || Objects.equals(letter, "О")) {
                    allLettersCounter++;
                    waitingCount++;
                }
                if (j >= 11 && i != 0)
                    str.append(" ");
                str.append(letter).append(" ");
            }
        }
        str.append("\n");
        str.append("turnaround time = ").append((double) allLettersCounter / processes.length).append("\n");
        str.append("waiting time = ").append((double) waitingCount / processes.length).append("\n");
        return str.toString();
    }
}
