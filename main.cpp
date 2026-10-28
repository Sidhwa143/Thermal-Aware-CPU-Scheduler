
#include <iostream>
#include <iomanip>
#include "scheduler.h"

using namespace std;

int main() {
    int n;

    cout << "Number of processes (1-"
         << MAX_PROCESSES << "): ";

    if (!(cin >> n) || n < 1 || n > MAX_PROCESSES) {
        cout << "Invalid number of processes.\n";
        return 1;
    }

    Process normalProcesses[MAX_PROCESSES];
    Process thermalProcesses[MAX_PROCESSES];

    for (int i = 0; i < n; i++) {
        Process p{};

        p.id = i + 1;

        cout << "P" << i + 1
             << " arrival time, burst time, priority: ";

        if (!(cin >> p.arrivalTime
                  >> p.burstTime
                  >> p.priority) ||
            p.arrivalTime < 0 ||
            p.burstTime < 1 ||
            p.priority < 1) {
            cout << "Invalid input.\n";
            return 1;
        }

        p.remainingTime = p.burstTime;
        p.startTime = -1;
        p.completionTime = 0;
        p.waitingTime = 0;
        p.turnaroundTime = 0;
        p.assignedCore = -1;

        normalProcesses[i] = p;
        thermalProcesses[i] = p;
    }

    SchedulerResult normal =
        runScheduler(normalProcesses, n, false);

    SchedulerResult thermal =
        runScheduler(thermalProcesses, n, true);

    cout << fixed << setprecision(2);
    cout << "\nFINAL COMPARISON\n";

    cout << left << setw(30) << "Metric"
         << setw(18) << "Normal"
         << setw(18) << "Thermal-Aware" << "\n";

    cout << left << setw(30) << "Average Waiting Time"
         << setw(18) << normal.averageWaitingTime
         << setw(18) << thermal.averageWaitingTime << "\n";

    cout << left << setw(30) << "Average Turnaround Time"
         << setw(18) << normal.averageTurnaroundTime
         << setw(18) << thermal.averageTurnaroundTime << "\n";

    cout << left << setw(30) << "Maximum Temperature"
         << setw(18) << normal.maximumTemperature
         << setw(18) << thermal.maximumTemperature << "\n";

    return 0;
}
