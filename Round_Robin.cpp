/* ✅ My Recommendation
Stick with a single ready queue for simplicity if this is for learning, assignments, or single-core simulation.
Use both ready & running queues if you plan to:
Visualize the CPU scheduler (ready queue + running process on screen).
Extend the simulation to multiple CPUs or more complex scheduling (like adding I/O queues).
*/



#include <iostream>
#include <queue>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

class Process {
public:
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;

    Process(int id, int at, int bt) {
        pid = id;
        arrivalTime = at;
        burstTime = bt;
        remainingTime = bt;
        completionTime = waitingTime = turnaroundTime = 0;
    }
};

class RoundRobin {
private:
    vector<Process> processes;
    int timeQuantum;
    int contextSwitchTime;
    vector<pair<int, int>> ganttChart;
    int totalIdleTime = 0;

public:
    RoundRobin(vector<Process> p, int tq, int cs) : processes(p), timeQuantum(tq), contextSwitchTime(cs) {}

    void schedule() {
        queue<int> readyQueue;    // Processes ready to execute
        queue<int> runningQueue;  // Currently executing process (size = 1)
        int time = 0, completed = 0;
        vector<bool> inQueue(processes.size(), false);

        sort(processes.begin(), processes.end(), [](Process &a, Process &b) { return a.arrivalTime < b.arrivalTime; });

        readyQueue.push(0);
        inQueue[0] = true;

        cout << "\n========== ROUND ROBIN CPU SCHEDULING START ==========\n";

        while (!readyQueue.empty() || !runningQueue.empty()) {

            // Move process from ready to running if CPU is idle
            if (runningQueue.empty() && !readyQueue.empty()) {
                int current = readyQueue.front();
                readyQueue.pop();
                runningQueue.push(current);
            }

            int i = runningQueue.front();
            runningQueue.pop();

            // If CPU is idle until process arrives
            if (time < processes[i].arrivalTime) {
                totalIdleTime += (processes[i].arrivalTime - time);
                time = processes[i].arrivalTime;
            }

            int execTime = min(timeQuantum, processes[i].remainingTime);
            cout << "CPU executing P" << processes[i].pid << " from time " << time << " to " << time + execTime << "\n";
            ganttChart.push_back({processes[i].pid, time});
            time += execTime + contextSwitchTime;
            processes[i].remainingTime -= execTime;

            // If process finished, calculate completion/turnaround/waiting times
            if (processes[i].remainingTime == 0) {
                processes[i].completionTime = time - contextSwitchTime;
                processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
                processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
                cout << "Process P" << processes[i].pid << " completed at time " << processes[i].completionTime << "\n";
                completed++;
            }

            // Check for newly arrived processes and add them to readyQueue
            for (int j = 0; j < processes.size(); j++) {
                if (!inQueue[j] && processes[j].arrivalTime <= time && processes[j].remainingTime > 0) {
                    readyQueue.push(j);
                    inQueue[j] = true;
                }
            }

            // If process not finished, add it back to readyQueue
            if (processes[i].remainingTime > 0) {
                readyQueue.push(i);
            }

            // If both queues empty but some processes remaining, push next arriving process
            if (readyQueue.empty() && runningQueue.empty() && completed < processes.size()) {
                for (int j = 0; j < processes.size(); j++) {
                    if (processes[j].remainingTime > 0) {
                        readyQueue.push(j);
                        inQueue[j] = true;
                        break;
                    }
                }
            }
        }

        cout << "\n========== ALL PROCESSES COMPLETED ==========\n";
    }

    void displayResults() {
        double totalWT = 0, totalTAT = 0;
        int totalTime = 0;

        cout << "\n------------------- RESULTS -------------------\n";
        cout << left << setw(8) << "PID" 
             << setw(12) << "Arrival" 
             << setw(10) << "Burst" 
             << setw(15) << "Completion" 
             << setw(10) << "Waiting" 
             << "Turnaround\n";

        for (auto &p : processes) {
            cout << left << setw(8) << p.pid
                 << setw(12) << p.arrivalTime
                 << setw(10) << p.burstTime
                 << setw(15) << p.completionTime
                 << setw(10) << p.waitingTime
                 << p.turnaroundTime << "\n";

            totalWT += p.waitingTime;
            totalTAT += p.turnaroundTime;
            totalTime = max(totalTime, p.completionTime);
        }

        cout << fixed << setprecision(2);
        cout << "\nAverage Waiting Time: " << (totalWT / processes.size());
        cout << "\nAverage Turnaround Time: " << (totalTAT / processes.size());
        cout << "\nThroughput: " << (double)processes.size() / totalTime << " processes/unit time";
        double cpuUtil = ((totalTime - totalIdleTime) / (double)totalTime) * 100;
        cout << "\nCPU Utilization: " << cpuUtil << " %\n";

        cout << "\n------------------- GANTT CHART -------------------\n";
        for (auto &g : ganttChart) {
            cout << "P" << g.first << "[" << g.second << "-";

            for (auto &p : processes) {
                if (p.pid == g.first) {
                    int endTime = min(g.second + timeQuantum, g.second + p.burstTime);
                    cout << g.second + (p.burstTime - p.remainingTime < timeQuantum ? p.burstTime - p.remainingTime + g.second : g.second + timeQuantum) << "] ";
                    break;
                }
            }
        }
        cout << "\n--------------------------------------------------\n";
    }
};

int main() {
    cout << "=== ROUND ROBIN CPU SCHEDULING SIMULATOR ===\n\n";
    int n, tq, cs;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> processes;
    cout << "Enter Arrival Time and Burst Time for each process\n(example: 0 5 means AT=0, BT=5)\n\n";
    for (int i = 0; i < n; i++) {
        int at, bt;
        cout << "Process " << i + 1 << ": ";
        cin >> at >> bt;
        processes.push_back(Process(i + 1, at, bt));
    }

    cout << "Enter Time Quantum: ";
    cin >> tq;

    cout << "Enter Context Switch Time (0 if none): ";
    cin >> cs;

    RoundRobin rr(processes, tq, cs);
    rr.schedule();
    rr.displayResults();

    return 0;
}
