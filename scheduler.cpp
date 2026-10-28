
#include "scheduler.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <climits>

using namespace std;

// Temperature and scheduling settings
const double HEATING_RATE = 1.5;
const double COOLING_RATE = 1.0;
const double MIN_TEMPERATURE = 25.0;
const double MAX_TEMPERATURE = 100.0;
const double WORKLOAD_WEIGHT = 0.5;

// Run either normal or thermal-aware scheduling
SchedulerResult runScheduler(
    Process processes[],
    int n,
    bool thermalAware
) {
    SchedulerResult result = {0.0, 0.0, 0.0};

    if (n <= 0 || n > MAX_PROCESSES) {
        return result;
    }

    Core cores[CORES];
    bool scheduled[MAX_PROCESSES] = {false};

    int assignedCount = 0;
    int currentTime = 0;

    double totalWaitingTime = 0.0;
    double totalTurnaroundTime = 0.0;
    double maximumTemperature = 0.0;

    // Initialize all CPU cores
    for (int c = 0; c < CORES; c++) {
        cores[c].temperature = 45.0 + c * 3.0;
        cores[c].currentProcess = -1;
        cores[c].busyUntil = 0;
        cores[c].totalWorkload = 0;

        maximumTemperature = max(
            maximumTemperature,
            cores[c].temperature
        );
    }

    cout << fixed << setprecision(2);

    cout << "\n====================================\n";
    cout << (thermalAware
        ? "THERMAL-AWARE SCHEDULING"
        : "NORMAL CPU SCHEDULING");
    cout << "\n====================================\n";

    while (true) {

        // Release cores whose processes have completed
        for (int c = 0; c < CORES; c++) {
            if (cores[c].currentProcess != -1 &&
                cores[c].busyUntil <= currentTime) {
                cores[c].currentProcess = -1;
            }
        }

        // Check whether any core is still busy
        bool anyCoreBusy = false;

        for (int c = 0; c < CORES; c++) {
            if (cores[c].currentProcess != -1) {
                anyCoreBusy = true;
                break;
            }
        }

        // Stop when all processes have finished
        if (assignedCount == n && !anyCoreBusy) {
            break;
        }

        // Select available processes by priority.
        // Priority 1 is the highest priority.
        // Ties: earlier arrival time, then lower process ID.
        while (true) {
            int bestProcess = -1;

            for (int i = 0; i < n; i++) {
                if (scheduled[i] ||
                    processes[i].arrivalTime > currentTime) {
                    continue;
                }

                if (bestProcess == -1 ||
                    processes[i].priority <
                        processes[bestProcess].priority ||
                    (processes[i].priority ==
                        processes[bestProcess].priority &&
                     processes[i].arrivalTime <
                        processes[bestProcess].arrivalTime) ||
                    (processes[i].priority ==
                        processes[bestProcess].priority &&
                     processes[i].arrivalTime ==
                        processes[bestProcess].arrivalTime &&
                     processes[i].id <
                        processes[bestProcess].id)) {
                    bestProcess = i;
                }
            }

            // No ready process is available
            if (bestProcess == -1) {
                break;
            }

            int bestCore = -1;
            double bestScore = 1e100;

            // Find the best available core
            for (int c = 0; c < CORES; c++) {
                if (cores[c].currentProcess != -1) {
                    continue;
                }

                int duration = processes[bestProcess].burstTime;

                // Estimate the temperature after this process
                double predictedTemperature = min(
                    MAX_TEMPERATURE,
                    cores[c].temperature +
                        duration * HEATING_RATE
                );

                double score;

                if (thermalAware) {
                    // Thermal-aware mode:
                    // Consider current temperature, predicted
                    // temperature, and accumulated workload.
                    score =
                        cores[c].temperature +
                        0.5 * predictedTemperature +
                        WORKLOAD_WEIGHT *
                            (cores[c].totalWorkload + duration);
                } else {
                    // Normal mode: consider workload only
                    score = cores[c].totalWorkload;
                }

                if (score < bestScore) {
                    bestScore = score;
                    bestCore = c;
                }
            }

            // All cores are currently busy
            if (bestCore == -1) {
                break;
            }

            int p = bestProcess;
            int duration = processes[p].burstTime;

            // Calculate process timing
            processes[p].startTime = currentTime;
            processes[p].completionTime = currentTime + duration;

            processes[p].turnaroundTime =
                processes[p].completionTime -
                processes[p].arrivalTime;

            processes[p].waitingTime =
                processes[p].startTime -
                processes[p].arrivalTime;

            processes[p].remainingTime = 0;
            processes[p].assignedCore = bestCore;

            // Assign the process to the selected core
            cores[bestCore].currentProcess = processes[p].id;
            cores[bestCore].busyUntil =
                processes[p].completionTime;

            cores[bestCore].totalWorkload += duration;

            scheduled[p] = true;
            assignedCount++;

            totalWaitingTime += processes[p].waitingTime;
            totalTurnaroundTime += processes[p].turnaroundTime;

            cout << "Time " << currentTime
                 << ": P" << processes[p].id
                 << " | Priority " << processes[p].priority
                 << " | Core " << bestCore + 1
                 << " | Score " << bestScore << "\n";
        }

        // If all processes are assigned, advance to the
        // next completion event instead of skipping time.
        if (assignedCount == n) {
            int nextFinish = INT_MAX;

            for (int c = 0; c < CORES; c++) {
                if (cores[c].currentProcess != -1) {
                    nextFinish = min(
                        nextFinish,
                        cores[c].busyUntil
                    );
                }
            }

            if (nextFinish == INT_MAX) {
                break;
            }

            int elapsed = nextFinish - currentTime;

            if (elapsed > 0) {
                for (int c = 0; c < CORES; c++) {
                    if (cores[c].currentProcess != -1) {
                        cores[c].temperature = min(
                            MAX_TEMPERATURE,
                            cores[c].temperature +
                                HEATING_RATE * elapsed
                        );
                    } else {
                        cores[c].temperature = max(
                            MIN_TEMPERATURE,
                            cores[c].temperature -
                                COOLING_RATE * elapsed
                        );
                    }

                    maximumTemperature = max(
                        maximumTemperature,
                        cores[c].temperature
                    );
                }

                currentTime = nextFinish;
            }

            continue;
        }

        // Find the next process arrival or core completion
        int nextTime = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!scheduled[i] &&
                processes[i].arrivalTime > currentTime) {
                nextTime = min(
                    nextTime,
                    processes[i].arrivalTime
                );
            }
        }

        for (int c = 0; c < CORES; c++) {
            if (cores[c].currentProcess != -1 &&
                cores[c].busyUntil > currentTime) {
                nextTime = min(
                    nextTime,
                    cores[c].busyUntil
                );
            }
        }

        // No future event was found
        if (nextTime == INT_MAX) {
            break;
        }

        int elapsed = nextTime - currentTime;

        // Update core temperatures during the elapsed time
        for (int c = 0; c < CORES; c++) {
            if (cores[c].currentProcess != -1) {
                cores[c].temperature = min(
                    MAX_TEMPERATURE,
                    cores[c].temperature +
                        HEATING_RATE * elapsed
                );
            } else {
                cores[c].temperature = max(
                    MIN_TEMPERATURE,
                    cores[c].temperature -
                        COOLING_RATE * elapsed
                );
            }

            maximumTemperature = max(
                maximumTemperature,
                cores[c].temperature
            );
        }

        currentTime = nextTime;
    }

    // Display process results
    cout << "\nProcess Results\n";

    cout << left
         << setw(10) << "Process"
         << setw(10) << "Priority"
         << setw(8) << "AT"
         << setw(8) << "BT"
         << setw(8) << "Core"
         << setw(8) << "Start"
         << setw(8) << "CT"
         << setw(8) << "TAT"
         << setw(8) << "WT" << "\n";

    for (int i = 0; i < n; i++) {
        cout << left
             << setw(10) << processes[i].id
             << setw(10) << processes[i].priority
             << setw(8) << processes[i].arrivalTime
             << setw(8) << processes[i].burstTime
             << setw(8) << processes[i].assignedCore + 1
             << setw(8) << processes[i].startTime
             << setw(8) << processes[i].completionTime
             << setw(8) << processes[i].turnaroundTime
             << setw(8) << processes[i].waitingTime
             << "\n";
    }

    // Display average performance metrics
    result.averageWaitingTime = totalWaitingTime / n;
    result.averageTurnaroundTime = totalTurnaroundTime / n;
    result.maximumTemperature = maximumTemperature;

    cout << "\nAverage Waiting Time: "
         << result.averageWaitingTime << "\n";

    cout << "Average Turnaround Time: "
         << result.averageTurnaroundTime << "\n";

    // Display Gantt chart for every core
    cout << "\nCore Gantt Chart\n";

    for (int c = 0; c < CORES; c++) {
        cout << "Core " << c + 1 << ": ";

        bool printed[MAX_PROCESSES] = {false};
        bool first = true;

        for (int count = 0; count < n; count++) {
            int nextProcess = -1;

            for (int i = 0; i < n; i++) {
                if (!printed[i] &&
                    processes[i].assignedCore == c) {

                    if (nextProcess == -1 ||
                        processes[i].startTime <
                            processes[nextProcess].startTime) {
                        nextProcess = i;
                    }
                }
            }

            if (nextProcess == -1) {
                break;
            }

            if (!first) {
                cout << " | ";
            }

            cout << "P" << processes[nextProcess].id
                 << " (" << processes[nextProcess].startTime
                 << "-"
                 << processes[nextProcess].completionTime
                 << ")";

            printed[nextProcess] = true;
            first = false;
        }

        if (first) {
            cout << "Idle";
        }

        cout << "\n";
    }

    // Display final core report
    cout << "\nCore Report\n";

    double minimumWorkload = cores[0].totalWorkload;
    double maximumWorkload = cores[0].totalWorkload;

    for (int c = 0; c < CORES; c++) {
        cout << "Core " << c + 1
             << " | Workload: " << cores[c].totalWorkload
             << " | Final Temperature: "
             << cores[c].temperature << " C\n";

        minimumWorkload = min(
            minimumWorkload,
            static_cast<double>(cores[c].totalWorkload)
        );

        maximumWorkload = max(
            maximumWorkload,
            static_cast<double>(cores[c].totalWorkload)
        );
    }

    cout << "Workload Difference: "
         << maximumWorkload - minimumWorkload << "\n";

    cout << "Maximum Simulated Temperature: "
         << maximumTemperature << " C\n";

    return result;
}
