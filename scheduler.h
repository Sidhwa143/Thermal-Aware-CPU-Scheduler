
#ifndef SCHEDULER_H
#define SCHEDULER_H

const int CORES = 4;
const int MAX_PROCESSES = 100;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int startTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    int assignedCore;
    int priority;
};

struct Core {
    double temperature;
    int currentProcess;
    int busyUntil;
    int totalWorkload;
};

struct SchedulerResult {
    double averageWaitingTime;
    double averageTurnaroundTime;
    double maximumTemperature;
};

SchedulerResult runScheduler(
    Process processes[],
    int n,
    bool thermalAware
);

#endif
