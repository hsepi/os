package ru.zakhse.memory_managers;

import ru.zakhse.Process;

import java.util.*;

/**
 * Created by Zakhse on 20.03.17.
 */
public class FCFSManager extends MemoryManager {
    public FCFSManager(int memorySize, int quantum, ShedullingPolicy policy, MemoryAllocation allocation, Process[]
            processes) {
        super(memorySize, quantum, policy, allocation, processes);
    }

    @Override
    public void solve() {

        PriorityQueue<Process> allProcesses = new PriorityQueue<>((o1, o2) -> {
            if (o1.startTime > o2.startTime)
                return 1;
            if (o1.startTime < o2.startTime)
                return -1;
            return 0;
        });

        Queue<Process> runningProcesses = new LinkedList<>();

        int timeIndex = 0;

        Queue<Process> waitingProcesses = new LinkedList<>();
        for (int i = 0; i < processes.length; i++)
            allProcesses.add(processes[i]);

        int quant = 0;
        while (!isAllProcessFinished(processes)) {
            table[0][timeIndex + 1] = String.valueOf(timeIndex);

            if (!allProcesses.isEmpty() && allProcesses.element().startTime == timeIndex) {
                Process tmp = allProcesses.poll();
                tmp.setState(Process.State.WAITING);
                waitingProcesses.add(tmp);
            }

            List<Process> toDelete = new LinkedList<>();
            for (Process p : waitingProcesses)
                if (pushProcess(allocatedMemory, p, allocation)) {
                    p.setState(Process.State.READY);
                    runningProcesses.add(p);
                    toDelete.add(p);
                }
            waitingProcesses.removeAll(toDelete);

            runningProcesses.element().setState(Process.State.RUNNING);
            runningProcesses.element().cpuBurst--;

            for (int i = 1; i <= processes.length; i++) {
                table[i][timeIndex + 1] = processes[i - 1].getStringState();
            }

            if (runningProcesses.element().cpuBurst == 0) {
                runningProcesses.element().setState(Process.State.FINISHED);
                for (int i = 0; i < allocatedMemory.length; i++)
                    if (allocatedMemory[i] == runningProcesses.element())
                        allocatedMemory[i] = null;
                runningProcesses.remove();
            }
            timeIndex++;
        }
    }
}
