//oded didi 211913587
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_PROCESSES 1000
#define MAX_NAME_LENGTH 50
#define MAX_LINE_LENGTH 256
#define MAX_DESCRIPTION_LENGTH 100

//structure to hold process information
typedef struct process {
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    int arrivalTime;
    int burstTime;
    int priority;
    pid_t pid; 
    int original_index;
} Process;

Process extractProcess(char* line, int original_index) {
    Process p;
    //Parse the line to extract process information
    char* token = strtok(line, ",");
    if (token != NULL) {
        strncpy(p.name, token, MAX_NAME_LENGTH);
        token = strtok(NULL, ",");
    }
    if (token != NULL) {
        strncpy(p.description, token, MAX_DESCRIPTION_LENGTH);
        token = strtok(NULL, ",");
    }
    if (token != NULL) {
        p.arrivalTime = atoi(token);
        token = strtok(NULL, ",");
    }
    if (token != NULL) {
        p.burstTime = atoi(token);
        token = strtok(NULL, ",");
    }
    if (token != NULL) {
        p.priority = atoi(token);
    } else {
        // Default priority if not specified
        p.priority = 0;
    }
    p.original_index = original_index; // Store the original index
    p.pid = -1; // Initialize PID to -1
    return p;
}

Process* readData(char* fileName, int* numProcesses) {
    static Process processes[MAX_PROCESSES];
    // Open the csv file for reading
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    // Read the file line by line and extract process information
    char line[MAX_LINE_LENGTH];
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        // Process each line
       processes[i] = extractProcess(line, i);
       i ++;
    }

    fclose(file);
    // Set the number of processes read
    *numProcesses = i;
    return processes;
}
// Function to print the opening message
void printOpeningMessage(char* type) {
    printf("══════════════════════════════════════════════\n");
    printf(">> Scheduler Mode : %s\n", type);
    printf(">> Engine Status  : Initialized\n");
    printf("──────────────────────────────────────────────\n\n");
    fflush(stdout);
}
// Function to print the final message
void printFinalMessage(double averageTime, char* calcType) {
    printf("\n");
    printf("──────────────────────────────────────────────\n");
    printf(">> Engine Status  : Completed\n");
    printf(">> Summary        :\n");
    printf("   └─ %s : %.2f time units\n", calcType ,averageTime); 
    printf(">> End of Report\n");
    printf("══════════════════════════════════════════════\n");
    fflush(stdout);
}

void swap(Process* a, Process* b) {
    Process temp = *a;
    *a = *b;
    *b = temp;
}
// Function to sort processes by arrival time and priority
void sortByArriveAndPrior(Process* processes, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime) {
                swap(&processes[j], &processes[j + 1]);    
            }
            else if (processes[j].arrivalTime == processes[j + 1].arrivalTime) {
                if (processes[j].original_index > processes[j + 1].original_index) {
                    swap(&processes[j], &processes[j + 1]);
                }
            }
        }
    }
}
// Function to sort processes by priority
void sortByPriority(Process* processes, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            Process* p1 = &processes[j];
            Process* p2 = &processes[j + 1];

            if (
                (p1->priority > p2->priority) ||
                (p1->priority == p2->priority && p1->arrivalTime > p2->arrivalTime) ||
                (p1->priority == p2->priority && p1->arrivalTime == p2->arrivalTime &&
                 p1->original_index > p2->original_index)
            ) {
                swap(p1, p2);
            }
        }
    }
}
// Function to sort processes by burst time
void sortByBurstTime(Process* processes, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            Process* p1 = &processes[j];
            Process* p2 = &processes[j + 1];

            if (
                (p1->burstTime > p2->burstTime) ||
                (p1->burstTime == p2->burstTime && p1->arrivalTime > p2->arrivalTime) ||
                (p1->burstTime == p2->burstTime && p1->arrivalTime == p2->arrivalTime &&
                 p1->original_index > p2->original_index)
            ) {
                swap(p1, p2);
            }
        }
    }
}
// Signal handler for SIGALRM
void alarmHandler(int signum) {
   // just returns from pause
}

void pauseHandler(int signum) {
   pause(); 
}

void resumeHandler(int sig) {
    // just returns from pause
}

void initializeSignalHandlers() {
    // Block all signals except SIGALRM and SIGCHLD
    sigset_t blockSet;
    sigfillset(&blockSet);
    sigdelset(&blockSet, SIGALRM);
    sigdelset(&blockSet, SIGCHLD);
    sigprocmask(SIG_BLOCK, &blockSet, NULL);
    // Set up the signal handler for SIGALRM
    struct sigaction sa;
    sa.sa_handler = alarmHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);
}

// Function to run the FCFS scheduling algorithm
void FCFS(Process* processes, int n) {
    // Initialize signal handlers, and block all signals except SIGALRM and SIGCHLD
    initializeSignalHandlers();
    // Sort processes by arrival time and by order
    sortByArriveAndPrior(processes, n);
    // Print the opening message
    printOpeningMessage("FCFS");
    int currTime = 0;
    int waitingTime = 0;
    
    // Loop through each process
    for (int i = 0; i < n; i++) {
        // If the process has not arrived yet, print idle time
        if (processes[i].arrivalTime > currTime) {
            alarm(processes[i].arrivalTime - currTime);
            pause();
            printf("%d → %d: Idle.\n", currTime, processes[i].arrivalTime);
            currTime = processes[i].arrivalTime;
        }
        
        // Fork a new process to run the current process
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process - setup signal handlers like in Round Robin
            signal(SIGUSR1, pauseHandler);  // Pause signal
            signal(SIGUSR2, resumeHandler);  // Resume signal
            
            sigset_t unblockSet;
            sigemptyset(&unblockSet);
            sigaddset(&unblockSet, SIGCHLD);
            sigprocmask(SIG_UNBLOCK, &unblockSet, NULL);
            
            while (1) {
                pause();  // Wait to be resumed by parent
            }
        } else {
            // Parent process - control the child like in Round Robin
            processes[i].pid = pid;
            
            // Send signal to child to start running
            kill(pid, SIGUSR2);
            
            // Block SIGCHLD and wait for the burst time
            sigset_t blockSet, prevSet;
            sigemptyset(&blockSet);
            sigaddset(&blockSet, SIGCHLD);
            sigprocmask(SIG_BLOCK, &blockSet, &prevSet);
            
            alarm(processes[i].burstTime);
            pause();
            
            sigprocmask(SIG_SETMASK, &prevSet, NULL);
            
            // Stop the child process
            kill(pid, SIGUSR1);
            
            // Print the running message (parent controls this)
            printf("%d → %d: %s Running %s.\n", 
                currTime, 
                currTime + processes[i].burstTime,
                processes[i].name, 
                processes[i].description);
            fflush(stdout);
            
            // Update the current time and waiting time
            waitingTime += currTime - processes[i].arrivalTime;
            currTime += processes[i].burstTime;
            
            // Kill the child process since FCFS is non-preemptive
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
        }
    }
    // Print the final message with the average waiting time
    printFinalMessage((double)waitingTime / n, "Average Waiting Time");
    printf("\n");
    fflush(stdout);

}
// Function to run the SJF scheduling algorithm
// Function to run the SJF scheduling algorithm
void sjfScheduler(Process* processes, int n) {  
    // Initialize signal handlers, and block all signals except SIGALRM and SIGCHLD
    initializeSignalHandlers();
    // Sort processes by burst time and by order
    sortByBurstTime(processes, n);
    printOpeningMessage("SJF");
    
    // Create an array to hold remaining processes
    Process remainingProcesses[n];
    for (int i = 0; i < n; i++) {
        remainingProcesses[i] = processes[i];
    }
    
    // Initialize current time, waiting time, and remaining count
    int currTime = 0;
    int waitingTime = 0;
    int remainingCount = n;
    
    // Loop until all processes are completed
    while (remainingCount > 0){
        int found_one = 0;
        // Check for processes that have arrived and can be run
        for (int i = 0; i < remainingCount; i++) {
            // If the process has arrived, fork a new process to run it
            if (remainingProcesses[i].arrivalTime <= currTime) {
                found_one = 1;
                pid_t pid = fork();
                if (pid < 0) {
                    perror("Fork failed\n");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Child process - setup signal handlers like in FCFS and Round Robin
                    signal(SIGUSR1, pauseHandler);  // Pause signal
                    signal(SIGUSR2, resumeHandler);  // Resume signal
                    
                    sigset_t unblockSet;
                    sigemptyset(&unblockSet);
                    sigaddset(&unblockSet, SIGCHLD);
                    sigprocmask(SIG_UNBLOCK, &unblockSet, NULL);
                    
                    while (1) {
                        pause();  // Wait to be resumed by parent
                    }
                } else {
                    // Parent process - control the child like in FCFS and Round Robin
                    remainingProcesses[i].pid = pid;
                    
                    // Send signal to child to start running
                    kill(pid, SIGUSR2);
                    
                    // Block SIGCHLD and wait for the burst time
                    sigset_t blockSet, prevSet;
                    sigemptyset(&blockSet);
                    sigaddset(&blockSet, SIGCHLD);
                    sigprocmask(SIG_BLOCK, &blockSet, &prevSet);
                    
                    alarm(remainingProcesses[i].burstTime);
                    pause();
                    
                    sigprocmask(SIG_SETMASK, &prevSet, NULL);
                    
                    // Stop the child process
                    kill(pid, SIGUSR1);
                    
                    // Print the running message (parent controls this)
                    printf("%d → %d: %s Running %s.\n", 
                        currTime, 
                        currTime + remainingProcesses[i].burstTime,
                        remainingProcesses[i].name, 
                        remainingProcesses[i].description);
                    fflush(stdout);
                    
                    // Update the current time and waiting time
                    waitingTime += currTime - remainingProcesses[i].arrivalTime;
                    currTime += remainingProcesses[i].burstTime;
                    
                    // Kill the child process since SJF is non-preemptive
                    kill(pid, SIGKILL);
                    waitpid(pid, NULL, 0);
                    
                    // Remove the completed process from the remaining processes
                    for (int j = i; j < remainingCount - 1; j++) {
                        remainingProcesses[j] = remainingProcesses[j + 1];
                    }
                    remainingCount--;
                    break;
                }
            }
        }
        
        // If no processes have arrived yet, wait for the next process to arrive
        if (found_one == 0) {
            int best = -1;
            int next = -1;
            // Calculate the next process to arrive
            for (int j = 0; j < remainingCount; j++) {
                if (best == -1 || best > remainingProcesses[j].arrivalTime) {
                    next = j;
                    best = remainingProcesses[j].arrivalTime;
                }
            }
            // Set an alarm for the next process to arrive
            alarm(remainingProcesses[next].arrivalTime - currTime);
            pause();
            printf("%d → %d: Idle.\n", currTime, remainingProcesses[next].arrivalTime);
            fflush(stdout);
            currTime = remainingProcesses[next].arrivalTime;
        }
    }
    
    printFinalMessage((double)waitingTime / n, "Average Waiting Time");
    printf("\n");
    fflush(stdout);
}
// Function to run the Priority scheduling algorithm
// Function to run the Priority scheduling algorithm
void prioritySchrduler(Process* processes, int n) {
    // Initialize signal handlers, and block all signals except SIGALRM and SIGCHLD
    initializeSignalHandlers();
    // Sort processes by priority and by order
    sortByPriority(processes, n);
    printOpeningMessage("Priority");
    fflush(stdout);
    // Create an array to hold remaining processes
    Process remainingProcesses[n];
    for (int i = 0; i < n; i++) {
        remainingProcesses[i] = processes[i];
    }
    int currTime = 0;
    int waitingTime = 0;
    int remainingCount = n;
    // Loop until all processes are completed
    while (remainingCount > 0) {
        int found_one = 0;
        int i = 0;
        // Check for processes that have arrived and can be run
        for (; i < remainingCount; i++) {
            if (remainingProcesses[i].arrivalTime <= currTime) {
                found_one = 1;
                pid_t pid = fork();
                if (pid < 0) {
                    perror("Fork failed\n");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Child process - setup signal handlers like in FCFS, SJF and Round Robin
                    signal(SIGUSR1, pauseHandler);  // Pause signal
                    signal(SIGUSR2, resumeHandler);  // Resume signal
                    
                    sigset_t unblockSet;
                    sigemptyset(&unblockSet);
                    sigaddset(&unblockSet, SIGCHLD);
                    sigprocmask(SIG_UNBLOCK, &unblockSet, NULL);
                    
                    while (1) {
                        pause();  // Wait to be resumed by parent
                    }
                } else {
                    // Parent process - control the child like in FCFS, SJF and Round Robin
                    remainingProcesses[i].pid = pid;
                    
                    // Send signal to child to start running
                    kill(pid, SIGUSR2);
                    
                    // Block SIGCHLD and wait for the burst time
                    sigset_t blockSet, prevSet;
                    sigemptyset(&blockSet);
                    sigaddset(&blockSet, SIGCHLD);
                    sigprocmask(SIG_BLOCK, &blockSet, &prevSet);
                    
                    alarm(remainingProcesses[i].burstTime);
                    pause();
                    
                    sigprocmask(SIG_SETMASK, &prevSet, NULL);
                    
                    // Stop the child process
                    kill(pid, SIGUSR1);
                    
                    // Print the running message (parent controls this)
                    printf("%d → %d: %s Running %s.\n", 
                        currTime, 
                        currTime + remainingProcesses[i].burstTime,
                        remainingProcesses[i].name, 
                        remainingProcesses[i].description);
                    fflush(stdout);
                    
                    // Update the current time and waiting time
                    waitingTime += currTime - remainingProcesses[i].arrivalTime;
                    currTime += remainingProcesses[i].burstTime;
                    
                    // Kill the child process since Priority is non-preemptive
                    kill(pid, SIGKILL);
                    waitpid(pid, NULL, 0);
                    
                    // Remove the completed process from the remaining processes
                    for (int j = i; j < remainingCount - 1; j++) {
                        remainingProcesses[j] = remainingProcesses[j + 1];
                    }
                    remainingCount--;
                    break;
                }
            }
        }
        // If no processes have arrived yet, wait for the next process to arrive
        if (found_one == 0) {
            int best = -1;
            int next = -1;
            // Calculate the next process to arrive
            for (int j = 0; j < remainingCount; j++) {
                if (best == -1 || best > remainingProcesses[j].arrivalTime) {
                    next = j;
                    best = remainingProcesses[j].arrivalTime;
                }
            }
            // Set an alarm for the next process to arrive
            alarm(remainingProcesses[next].arrivalTime - currTime);
            pause();
            printf("%d → %d: Idle.\n", currTime, remainingProcesses[next].arrivalTime);
            fflush(stdout);
            currTime = remainingProcesses[next].arrivalTime;
        }
    }
    printFinalMessage((double)waitingTime / n, "Average Waiting Time"); 
    printf("\n");
    fflush(stdout); 
}

// Function to run a process with the given time quantum
void runProcess(Process* process, int timeQuantum, int* currentTime, int isFirstTime) {
    if (isFirstTime) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
            // In child process
        } else if (pid == 0) {
            signal(SIGUSR1, pauseHandler);  // Pause signal
            signal(SIGUSR2, resumeHandler);  // Resume signal
            while (1) {
                pause();  // Wait to be resumed
            }
        } else {
            process->pid = pid;
        }
    }
    // Set the execution time for the process
    int execTime = (process->burstTime > timeQuantum) ? timeQuantum : process->burstTime;
    // Send signals to the process to start running
    kill(process->pid, SIGUSR2); 
    // Block SIGCHLD to wait for the process to finish     
    sigset_t blockSet, prevSet;
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGCHLD);
    sigprocmask(SIG_BLOCK, &blockSet, &prevSet);
    alarm(execTime);               
    pause();                     
    sigprocmask(SIG_SETMASK, &prevSet, NULL); 
    // Stop the process to run
    kill(process->pid, SIGUSR1);      
    // Print the running message
    printf("%d → %d: %s Running %s.\n",
           *currentTime, *currentTime + execTime, process->name, process->description);
    fflush(stdout);
    // Update the process burst time and current time
    process->burstTime -= execTime;
    *currentTime += execTime;
    // If the process has finished, kill it and wait for it to finish
    if (process->burstTime <= 0) {
        kill(process->pid, SIGKILL);
        waitpid(process->pid, NULL, 0);
    } else {
        process->arrivalTime = *currentTime;
    }
}

// Function to run the Round Robin scheduling algorithm
void roundRobin(Process* processes, int numOfProcesses, int timeQuantum) {
    initializeSignalHandlers();
    sortByArriveAndPrior(processes, numOfProcesses);
    printOpeningMessage("Round Robin");

    int currentTime = 0;
    int nextIdx = 0;
    Process* readyQueue[numOfProcesses];
    int head = 0, tail = 0, count = 0;

    while (nextIdx < numOfProcesses || count > 0) {
        // Check if there's a process that arrived and should run before the readyQueue head
        int shouldRunFromIncoming = 0;
        if (nextIdx < numOfProcesses && processes[nextIdx].arrivalTime <= currentTime) {
            if (count == 0 || processes[nextIdx].arrivalTime < readyQueue[head]->arrivalTime) {
                shouldRunFromIncoming = 1;
            }
        }
        // If there's a process that should run from incoming processes, run it
        if (shouldRunFromIncoming) {
            Process* curr = &processes[nextIdx++];
            int isFirstTime = (curr->pid == -1);
            runProcess(curr, timeQuantum, &currentTime, isFirstTime);
            // If the process still has burst time left, add it back to the ready queue
            if (curr->burstTime > 0) {
                curr->arrivalTime = currentTime;
                if (count < numOfProcesses) {
                    readyQueue[tail] = curr;
                    tail = (tail + 1) % numOfProcesses;
                    count++;
                }
            }
        // If there's a process in the ready queue, run it
        } else if (count > 0) {
            Process* curr = readyQueue[head];
            head = (head + 1) % numOfProcesses;
            count--;
            int isFirstTime = (curr->pid == -1);
            runProcess(curr, timeQuantum, &currentTime, isFirstTime);
            // If the process still has burst time left, add it back to the ready queue
            if (curr->burstTime > 0) {
                curr->arrivalTime = currentTime;
                if (count < numOfProcesses) {
                    readyQueue[tail] = curr;
                    tail = (tail + 1) % numOfProcesses;
                    count++;
                }
            }
        // If no process is ready to run, wait for the next process to arrive
        } else if (nextIdx < numOfProcesses) {
            int waitTime = processes[nextIdx].arrivalTime - currentTime;

            sigset_t blockSet, prevSet;
            sigemptyset(&blockSet);
            sigaddset(&blockSet, SIGCHLD);
            sigprocmask(SIG_BLOCK, &blockSet, &prevSet);

            alarm(waitTime);
            pause();

            sigprocmask(SIG_SETMASK, &prevSet, NULL);

            printf("%d → %d: Idle.\n", currentTime, currentTime + waitTime);
            fflush(stdout);
            currentTime += waitTime;
        }
    }
    // Print the final message with the total turnaround time
    printf("\n");
    printf("──────────────────────────────────────────────\n");
    printf(">> Engine Status  : Completed\n");
    printf(">> Summary        :\n");
    printf("   └─ Total Turnaround Time : %d time units\n\n",currentTime); 
    printf(">> End of Report\n");
    printf("══════════════════════════════════════════════\n");
    fflush(stdout);
}

// Function to run the CPU Scheduler
void runCPUScheduler(char* processesCsvFilePath, int timeQuantum) {
    int numProcesses;
    Process* processes = readData(processesCsvFilePath, &numProcesses);
    
    if (numProcesses == 0) {
        printf("No processes found in the file.\n");
        return;
    }
    // Run fcfsSchduler scheduling
    FCFS(processes, numProcesses);
    // Run sjfSchduler scheduling
    sjfScheduler(processes, numProcesses);
    // Run prioritySchrduler scheduling
    prioritySchrduler(processes, numProcesses);
    // Run roundRobin scheduling
    roundRobin(processes, numProcesses, timeQuantum);
    printf("\n");
    
}
