package ru.zakhse;

import ru.zakhse.memory_managers.ManagerFabric;
import ru.zakhse.memory_managers.MemoryManager;

import java.util.LinkedList;
import java.util.List;

public class Main {
    // 1st arg - memory size
    // 2nd arg - quantum
    // 3rd arg - N of processes
    // next N combinations of three args - process's start time, CPUBurst and memory
    public static void main(String[] args) {
        try {

            int memorySize;
            int quantum;
            int NOfProcesses;
            Process[] processes;

            try {
                memorySize = Integer.parseInt(args[0]);

                quantum = Integer.parseInt(args[1]);
                NOfProcesses = Integer.parseInt(args[2]);

                if (args.length != 3 + NOfProcesses * 3) {
                    System.out.println("Invalid number of arguments");
                    return;
                }
                processes = new Process[NOfProcesses];
                for (int i = 0; i < NOfProcesses; i++) {
                    int startTime = Integer.parseInt(args[2 + i * 3 + 1]);
                    int cpuBurst = Integer.parseInt(args[2 + i * 3 + 2]);
                    int memory = Integer.parseInt(args[2 + i * 3 + 3]);
                    processes[i] = new Process(i + 1, startTime, cpuBurst, memory);
                }
            } catch (NumberFormatException | ArrayIndexOutOfBoundsException e) {
                System.out.println("Invalid data, it should contains numbers");
                return;
            }

            List<MemoryManager> managers = new LinkedList<>();

            ManagerFabric fabric = new ManagerFabric(memorySize, quantum, processes);

            for (MemoryManager.ShedullingPolicy policy : MemoryManager.ShedullingPolicy.values()) {
                for (MemoryManager.MemoryAllocation allocation : MemoryManager.MemoryAllocation.values()) {
                    MemoryManager m = fabric.getManager(policy, allocation);
                    managers.add(m);
                }
            }

            for (MemoryManager m : managers) {
                m.solve();
                System.out.println(m.toString());
                System.out.println("==================================\n");
            }
        } catch (Exception e) {e.printStackTrace();}
    }
}
