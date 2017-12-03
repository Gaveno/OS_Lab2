/*
 * CPU Schedular Simulation Program
 * Author: Gavin Atkin
 * Created for Lab# 2 - Operating System Concepts
 */
 
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "dlinklist.h"
#include "queue.h"

#define NOT_RESPONDED 999999

using namespace std;

typedef double timetype;

// The data structure to hold process information.
struct PCB {
    unsigned int pid;
    unsigned int arrival_time;
    unsigned int burst_time;
    unsigned int priority;
    timetype waiting;
    timetype finish;
    timetype turn_around;
    timetype response;
    timetype remaining_time;
    timetype time_in;
    int num_context;
};

// Initial PCB linked list
DLinkedList<PCB> pcb;
// Current list of processes that have arrived and not finished
Queue<PCB> queue;
// List of finished processes
Queue<PCB> finished;
// time the algorithm has run 
timetype runtime;
timetype runtime_prev;

/* Comparison functions for insertion and sort.
 */
bool greaterthan_arrival(const PCB& left, const PCB& right) {
    return (left.arrival_time > right.arrival_time);
}

bool greaterthan_remaining(const PCB& left, const PCB& right) {
    return (left.remaining_time > right.remaining_time);
}

bool greaterthan_priority(const PCB& left, const PCB& right) {
    return (left.priority > right.priority);
}

bool greaterthan_pid(const PCB& left, const PCB& right) {
    return (left.pid > right.pid);
}


// Function prototypes.
bool stringIsValidLine(const string& str);
unsigned int stringCountNumbers(const string& str);
int stringGetNthNumber(const string& str, unsigned int n);
ostream& printResults(ostream& os, int mode, int time_q = 0);
void printQueue( );

// Method to update wait times of processes.
void incWaitingTime(int start = 1) {
    // Update waiting time
    for (unsigned int i = start; i < queue.getSize(); ++i) {
        queue.myList[i].waiting += 0.5f;
    }
}

/*  Add new arrivals based on algorithm
 */
void addArrivals(int mode) {
    unsigned int i = 0;
    while (i < pcb.getSize()) {
        if (pcb[i].arrival_time <= runtime) {
            switch (mode) {
                // First Come First Serve
                case 0:
                queue.enqueue(pcb[i]);
                break;
                // Shortest Remaining Time First
                case 1:
                {
                    PCB *front;
                    if (queue.getSize() > 0) {
                        front = &queue.getFront();
                    }
                    queue.enqueue(pcb[i], greaterthan_remaining);
                    if (queue.getFront().pid == pcb[i].pid && queue.getSize() > 1) {
                        runtime += 0.5f;
                        for (unsigned int i = 0; i < queue.getSize(); ++i) {
                            queue.myList[i].waiting += 0.5f;
                        }
                    }
                    //cout << "new front pid: " << queue.getFront().pid << endl;
                    if (queue.getSize() > 1 && front->pid != queue.getFront().pid) {
                        //cout << "updating context" << endl;
                        front->num_context++;
                    }
                }
                //queue.enqueue(pcb[i]);
                break;
                // Round Robin
                case 2:
                queue.enqueue(pcb[i]);
                break;
                // Preemptive Priority
                case 3:
                {
                    PCB *front;
                    if (queue.getSize() > 0) {
                        front = &queue.getFront();
                        //cout << "queue size greater than 0" << endl;
                        //cout << "front id: " << front->pid << endl;
                    }
                    queue.enqueue(pcb[i], greaterthan_priority);
                    if (queue.getFront().pid == pcb[i].pid && queue.getSize() > 1) {
                        runtime += 0.5f;
                        for (unsigned int i = 0; i < queue.getSize(); ++i) {
                            queue.myList[i].waiting += 0.5f;
                        }
                    }
                    //cout << "new front pid: " << queue.getFront().pid << endl;
                    if (queue.getSize() > 1 && front->pid != queue.getFront().pid) {
                        //cout << "updating context" << endl;
                        front->num_context++;
                    }
                }
                break;
                
            }
            pcb.remove(i);
        }
        else {
            i++;
        }
    }
}

void stepCPU_FCFS( ) {
    
    // Add new arrivals based on mode
    addArrivals(0);
    
    for (unsigned int i = 1; i < queue.getSize(); ++i) {
        queue.myList[i].waiting += 1.0f;
    }
    
    runtime_prev = runtime;     // testing
    // If not responded to first in line, respond.
    if (queue.getFront().response == NOT_RESPONDED) {
        queue.getFront().response = runtime - queue.getFront().arrival_time;
        queue.getFront().waiting = queue.getFront().response;
    }
    // Update run time and remaining time.
    runtime += 1.0f;
    queue.getFront().remaining_time--;
    if (queue.getFront().remaining_time <= 0) {
        queue.getFront().finish = runtime;
        queue.getFront().turn_around = queue.getFront().finish - queue.getFront().arrival_time;
        PCB done = queue.dequeue();
        finished.enqueue(done);
        //cout << "process: " << done.pid << " finished" << endl;
    }
}

void stepCPU_SRTF( ) {
    
    // Add new arrivals based on mode
    addArrivals(1);
    
    //printQueue();         // Debug
    
    runtime_prev = runtime;
    if (queue.sort(greaterthan_remaining) > 0) {
        runtime += 0.5f;
        cout << "sort changed things" << endl;
    }
    
    if (queue.getFront().remaining_time <= 0) {
        queue.getFront().finish = runtime;
        queue.getFront().turn_around = queue.getFront().finish - queue.getFront().arrival_time;
        PCB done = queue.dequeue();
        finished.enqueue(done);
        //cout << "process: " << done.pid << " finished" << endl;
    }
    
    if (queue.getSize() > 0) {
        if (queue.getFront().response == NOT_RESPONDED) {
            queue.getFront().response = runtime - queue.getFront().arrival_time;
        }
        queue.getFront().remaining_time -= 0.5f;
    }
    runtime += 0.5f;
    // Update waiting time
    for (unsigned int i = 1; i < queue.getSize(); ++i) {
        queue.myList[i].waiting += 0.5f;
    }
}

void stepCPU_RR(int time_q) {
    
    // Add new arrivals based on mode
    addArrivals(2);
    
    //printQueue();         // Debug
    
    
    runtime_prev = runtime;
    if (queue.getFront().remaining_time <= 0) {
        queue.getFront().finish = runtime;
        queue.getFront().turn_around = queue.getFront().finish - queue.getFront().arrival_time;
        PCB done = queue.dequeue();
        finished.enqueue(done);
        //cout << "process: " << done.pid << " finished" << endl;
    }
    // Update waiting time
    incWaitingTime();
    
    // Is queue still non-empty?
    if (queue.getSize() > 0) {
        if (queue.getFront().response == NOT_RESPONDED) {
            // Set response time if not set yet
            queue.getFront().response = runtime - queue.getFront().arrival_time;
        }
        queue.getFront().remaining_time -= 0.5f;
        // manage time processing
        queue.getFront().time_in += 0.5f;
        if (queue.getFront().time_in >= time_q && queue.getFront().remaining_time > 0) {
            // Add context switch time
            runtime += 0.5f;
            // Everything is delayed by switch
            
            incWaitingTime(0);
            PCB front = queue.dequeue();
            front.time_in = 0;
            front.num_context++;
            queue.enqueue(front);
            
        }
    }
    runtime += 0.5f;
    
}

void stepCPU_PP( ) {
    
    // Add new arrivals based on mode
    addArrivals(3);
    
    //printQueue();         // Debug
    
    runtime_prev = runtime;
    /*if (queue.sort(greaterthan_remaining) > 0) {
        runtime += 0.5f;
        cout << "sort changed things" << endl;
    }*/
    
    if (queue.getFront().remaining_time <= 0) {
        queue.getFront().finish = runtime;
        queue.getFront().turn_around = queue.getFront().finish - queue.getFront().arrival_time;
        PCB done = queue.dequeue();
        finished.enqueue(done);
        //cout << "process: " << done.pid << " finished" << endl;
    }
    
    if (queue.getSize() > 0) {
        if (queue.getFront().response == NOT_RESPONDED) {
            queue.getFront().response = runtime - queue.getFront().arrival_time;
        }
        queue.getFront().remaining_time -= 0.5f;
    }
    runtime += 0.5f;
    // Update waiting time
    for (unsigned int i = 1; i < queue.getSize(); ++i) {
        queue.myList[i].waiting += 0.5f;
    }
}


int main(int argc, const char* argv[]) {
    // File to read values from.
    ifstream file;
    // File to output results to.
    ofstream ofile;
    // Exit program flag.
    bool exit = false;
    // line number for debug.
    int linenum = 0;
    // Time running
    runtime = 0.0f;
    runtime_prev = -1.0f;
    
    // First round of error checking.
    if ((argc < 4) || (stoi(argv[3]) == 2 && argc < 5)) {
        cout << "Error: Not enough arguments given.\n";
        exit = true;
    }
    
    file.open(argv[1]);
    ofile.open(argv[2]);
    
    // Create process structure
    while (!exit && !file.eof()) {
        PCB newprocess;
        string line;
        getline(file, line);
        if (stringIsValidLine(line) && stringCountNumbers(line) == 4) {
            newprocess.pid = stringGetNthNumber(line, 0);
            newprocess.arrival_time = stringGetNthNumber(line, 1);
            newprocess.remaining_time = stringGetNthNumber(line, 2);
            newprocess.priority = stringGetNthNumber(line, 3);
            
            newprocess.waiting = 0;
            newprocess.finish = 0;
            newprocess.turn_around = 0;
            newprocess.response = NOT_RESPONDED;
            newprocess.num_context = 0;
            newprocess.burst_time = newprocess.remaining_time;
            newprocess.time_in = 0;
            pcb.addBack(newprocess);
        }
        else {
            if (line.length() > 1) {
                cout << "Error: bad input file.\n";
                cout << "line: " << linenum << "\n" << line << endl;
                cout << "valid: ";
                if (stringIsValidLine(line))
                    cout << "true\n";
                else
                    cout << "false\n";
                cout << "number count: " << stringCountNumbers(line) << endl << endl;
                exit = true;
            }
        }
        linenum++;
    }
    

    // Update queues based on algorithm.
    while (pcb.getSize() > 0 || queue.getSize() > 0) {
        switch (stoi(argv[3])) {
            case 0:
            stepCPU_FCFS();
            break;
            case 1:
            stepCPU_SRTF();
            break;
            case 2:
            stepCPU_RR(stoi(argv[4]));
            break;
            case 3:
            stepCPU_PP();
            break;
            default:
            pcb.removeFront();
        }
    }
    
    // Print out results to console and to file.
    if (argc > 4) {
        printResults(cout, stoi(argv[3]), stoi(argv[4]));
        printResults(ofile, stoi(argv[3]), stoi(argv[4]));
    }
    else {
        printResults(cout, stoi(argv[3]));
        printResults(ofile, stoi(argv[3]));
    }
    
    file.close();
    ofile.close();
    
    return 0;
}

/* The following 5 functions are for calculating averages
 * or combining totals.
 */
timetype getAverageBurst() {
    timetype avg = 0;
    for (unsigned int i = 0; i < finished.getSize(); ++i) {
        avg += finished.myList[i].burst_time;
    }
    avg /= finished.getSize();
    return avg;
}

timetype getAverageWaiting() {
    timetype avg = 0;
    for (unsigned int i = 0; i < finished.getSize(); ++i) {
        avg += finished.myList[i].waiting;
    }
    avg /= finished.getSize();
    return avg;
}

timetype getAverageTurnaround() {
    timetype avg = 0;
    for (unsigned int i = 0; i < finished.getSize(); ++i) {
        avg += finished.myList[i].turn_around;
    }
    avg /= finished.getSize();
    return avg;
}

timetype getAverageResponse() {
    timetype avg = 0;
    for (unsigned int i = 0; i < finished.getSize(); ++i) {
        avg += finished.myList[i].response;
    }
    avg /= finished.getSize();
    return avg;
}

timetype getTotalContext() {
    int tot = 0;
    for (unsigned int i = 0; i < finished.getSize(); ++i) {
        tot += finished.myList[i].num_context;
    }
    return tot;
}

// Print ensuring maximum width is met.
ostream& printWidth(ostream& os, string str, unsigned int width) {
    os << str;
    if (str.length() < width) {
        for (unsigned int i = 0; i < width - str.length(); ++i) {
            os << " ";
        }
    }
    return os;
}

/*  Print final data table or save them to a file.
*/
ostream& printResults(ostream& os, int mode, int time_q) {
    os << "*********************************************************************" << endl;
    os << "*********     Scheduling algorithm: ";
    switch (mode) {
        case 0: os << "FCFS               "; break;
        case 1: os << "SRTF               "; break;
        case 2: os << "Round Robin        "; break;
        case 3: os << "Preemptive Priority"; break;
    }
    os << "**************" << endl;
    if (mode == 2) {
        os << "*********     (No. of Task = " << finished.getSize() << " Quantum = " << time_q;
        os << ")            **************\n";
    }
    os << "*********************************************************************" << endl << endl;
    //int spacing = 15;
    timetype avg_burst = getAverageBurst();
    timetype avg_wait = getAverageWaiting();
    timetype avg_turnaround = getAverageTurnaround();
    timetype avg_response = getAverageResponse();
    int tot_context = getTotalContext();
    //os << setw(1) << "pid" << setw(spacing) << "arrival" << setw(spacing) << "CPU-burst" << setw(spacing) << "priority";
    //os << setw(spacing) << "finish" << setw(spacing) << "waiting time" << setw(spacing) << "turn around" << setw(spacing) << "response time";
    //os << setw(spacing) << "No. of Context" << endl;
    os << "| pid | arrival | CPU-burst | priority | finish | waiting time | turn around | response time | No. of Context |" << endl;
    os << "---------------------------------------------------------------------------------------------------------------" << endl;
    finished.sort(greaterthan_pid);
    
    for (unsigned int i = 0; i < finished.getSize(); ++i) {
        PCB current = finished.myList[i];
        /*os << fixed << setprecision(0);
        os << setw(1) << current.pid << setw(spacing) << current.arrival_time << setw(spacing) << current.burst_time;
        os << setw(spacing) << current.priority;
        os << fixed << setprecision(1);
        os << setw(spacing) << current.finish << setw(spacing) << current.waiting;
        os << setw(spacing) << current.turn_around << setw(spacing) << current.response << setw(spacing) << current.num_context;*/
        printWidth(os, "| " + to_string(current.pid).substr(0, 6), 6);
        printWidth(os, "| " + to_string(current.arrival_time).substr(0, 6), 10);
        printWidth(os, "| " + to_string(current.burst_time).substr(0, 6), 12);
        printWidth(os, "| " + to_string(current.priority).substr(0, 6), 11);
        printWidth(os, "| " + to_string(current.finish).substr(0, 6), 9);
        printWidth(os, "| " + to_string(current.waiting).substr(0, 6), 15);
        printWidth(os, "| " + to_string(current.turn_around).substr(0, 6), 14);
        printWidth(os, "| " + to_string(current.response).substr(0, 6), 16);
        printWidth(os, "| " + to_string(current.num_context).substr(0, 6), 17);
        os << "|" << endl;
        os << "---------------------------------------------------------------------------------------------------------------" << endl;
    }
    os << endl << setprecision(3);
    os << "Average CPU burst time = " << avg_burst << " ms,    ";
    os << "Average waiting time = " << avg_wait << " ms\n";
    os << "Average turn around time = " << avg_turnaround << " ms,    ";
    os << "Average response time = " << avg_response << " ms\n";
    os << "Total No. of Context Switching Performed = " << tot_context << endl << endl;
    return os;
}

// Debug function to track what is changing.
void printQueue( ) {
    int spacing = 15;
    cout << "runtime: " << runtime << endl;
    cout << setw(1) << "pid" << setw(spacing) << "arrival" << setw(spacing) << "CPU-remaining";
    cout << setw(spacing) << "waiting time" << setw(spacing) << "turn around" << setw(spacing) << "response time";
    cout << setw(spacing) << "No. of Context" << setw(spacing) << "time in" << endl;
    for (unsigned int i = 0; i < queue.getSize(); ++i) {
        PCB current = queue.myList[i];
        cout << setw(1) << current.pid << setw(spacing) << current.arrival_time << setw(spacing) << current.remaining_time;
        cout << setw(spacing) << current.waiting;
        cout << setw(spacing) << current.turn_around << setw(spacing) << current.response << setw(spacing) << current.num_context;
        cout << setw(spacing) << current.time_in << endl;
    }
}

bool stringIsValidLine(const string& str) {
    bool pass = true;
    for (unsigned int i = 0; i < str.length(); i++) {
        if (str[i] != 32 && str[i] != 13 && str[i] != '\n' && (str[i] < 48 || str[i] > 57)
            && str[i] != 45) {
            pass = false;
        }
    }
    return pass;
}

unsigned int stringCountNumbers(const string& str) {
    unsigned int num = 0;
    char previous = 32;
    for (unsigned int i = 0; i < str.length(); i++) {
        if (((str[i] >= 48 && str[i] <= 57) || str[i] == 45) && previous == 32) ++num;
        previous = str[i];
    }
    return num;
}

int stringGetNthNumber(const string& str, unsigned int n) {
    int current = -1;
    stringstream ss(str);
    string value;
    do {
        ss >> value;
        current++;
    } while (!ss.eof() && current < (int)n);
    
    return stoi(value);
}