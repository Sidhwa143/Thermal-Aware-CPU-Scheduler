
#include <iostream>
#include <iomanip>
#include "scheduler.h"

using namespace std;

int main() {
    int n;

    if (!(cin >> n) || n < 1 || n > MAX_PROCESSES) {
        cerr << "Invalid number of processes";
        return 1;
    }

    Process normalProcesses[MAX_PROCESSES];
    Process thermalProcesses[MAX_PROCESSES];

    for (int i = 0; i < n; i++) {
        int arrival, burst, priority;

        if (!(cin >> arrival >> burst >> priority) ||
            arrival < 0 || burst < 1 || priority < 1) {
            cerr << "Invalid process input";
            return 1;
        }

        Process p{};
        p.id = i + 1;
        p.arrivalTime = arrival;
        p.burstTime = burst;
        p.remainingTime = burst;
        p.startTime = -1;
        p.completionTime = 0;
        p.waitingTime = 0;
        p.turnaroundTime = 0;
        p.assignedCore = -1;
        p.priority = priority;

        normalProcesses[i] = p;
        thermalProcesses[i] = p;
    }

    SchedulerResult normal =
        runScheduler(normalProcesses, n, false);

    SchedulerResult thermal =
        runScheduler(thermalProcesses, n, true);

    cout << fixed << setprecision(2);

    cout << "RESULTS_START\n";
    cout << normal.averageWaitingTime << "\n";
    cout << normal.averageTurnaroundTime << "\n";
    cout << normal.maximumTemperature << "\n";
    cout << thermal.averageWaitingTime << "\n";
    cout << thermal.averageTurnaroundTime << "\n";
    cout << thermal.maximumTemperature << "\n";
    cout << "RESULTS_END\n";

    cout << "GANTT_START\n";
    cout << "NORMAL\n";

    for (int i = 0; i < n; i++) {
        cout << normalProcesses[i].id << " "
             << normalProcesses[i].assignedCore + 1 << " "
             << normalProcesses[i].startTime << " "
             << normalProcesses[i].completionTime << "\n";
    }

    cout << "THERMAL\n";

    for (int i = 0; i < n; i++) {
        cout << thermalProcesses[i].id << " "
             << thermalProcesses[i].assignedCore + 1 << " "
             << thermalProcesses[i].startTime << " "
             << thermalProcesses[i].completionTime << "\n";
    }

    cout << "GANTT_END\n";

    return 0;
}
