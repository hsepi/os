package ru.zakhse.memory_managers;

import ru.zakhse.Process;

import java.util.*;

public class SJFManager extends MemoryManager {
    public SJFManager(int memorySize, int quantum, ShedullingPolicy policy, MemoryAllocation allocation, Process[]
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

        Queue<Process> runningProcesses = new PriorityQueue<>((o1, o2) -> {
            if (o1.cpuBurst > o2.cpuBurst)
                return 1;
            if (o1.cpuBurst < o2.cpuBurst)
                return -1;
            return 0;
        });

        int timeIndex = 0;

        Queue<Process> waitingProcesses = new LinkedList<>();
        for (int i = 0; i < processes.length; i++)
            allProcesses.add(processes[i]);

        Process runningProcess = null;
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


            if (runningProcess == null)
                runningProcess = runningProcesses.element();
            runningProcess.setState(Process.State.RUNNING);
            runningProcess.cpuBurst--;


            for (int i = 1; i <= processes.length; i++) {
                table[i][timeIndex + 1] = processes[i - 1].getStringState();
            }

            if (runningProcess.cpuBurst == 0) {
                runningProcess.setState(Process.State.FINISHED);
                for (int i = 0; i < allocatedMemory.length; i++)
                    if (allocatedMemory[i] == runningProcess)
                        allocatedMemory[i] = null;
                runningProcesses.remove(runningProcess);
                runningProcess = null;
            }
            timeIndex++;
        }
    }
}
