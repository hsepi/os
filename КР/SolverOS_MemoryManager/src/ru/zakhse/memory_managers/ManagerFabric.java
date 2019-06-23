package ru.zakhse.memory_managers;

import ru.zakhse.Process;

public class ManagerFabric {
    public ManagerFabric(int memorySize, int quantum, Process[] processes) {
        this.memorySize = memorySize;
        this.quantum = quantum;
        this.processes = processes;
    }

    public MemoryManager getManager(MemoryManager.ShedullingPolicy policy, MemoryManager.MemoryAllocation allocation) {
        switch (policy) {
            case RR:
                return new RRManager(memorySize, quantum, policy, allocation, processes);
            case FCFS:
                return new FCFSManager(memorySize, quantum, policy, allocation, processes);
            case SJF:
                return new SJFManager(memorySize, quantum, policy, allocation, processes);
            case SJFPreemptive:
                return new SJFPreemptiveManager(memorySize, quantum, policy, allocation, processes);

        }
        return null;
    }

    private int memorySize;
    private int quantum;
    private Process[] processes;
}
