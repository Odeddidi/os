# OS Exercise 03 (`ex3`)

This exercise contains one executable (`ex3.c`) with **two modes**:
- **Focus Mode**: simulates “distractions” using UNIX signals.
- **CPU Scheduler**: simulates several CPU scheduling algorithms on a list of processes read from a CSV.

## Files

- `main.c.c`: program entry point (dispatches to Focus Mode or CPU Scheduler)
- `Focus-Mode.c`: Focus Mode implementation (`runFocusMode`)
- `CPU-Scheduler.c`: CPU Scheduler implementation (`runCPUScheduler`)
- `tester.csv`: sample process input for the scheduler

## Build

From the repo root:

```sh
gcc -Wall -Wextra -O2 -o ex3_app CPU-Scheduler/main.c.c
```

Note: `main.c.c` directly `#include`s `Focus-Mode.c` and `CPU-Scheduler.c`, so compiling `main.c.c` is sufficient.

## Run

General usage (as printed by the program):

```text
./ex3_app <Focus-Mode/CPU-Schedule> <Num-Of-Rounds/Processes.csv> <Round-Duration/Time-Quantum>
```

### 1) Focus Mode

Run:

```sh
./ex3_app Focus-Mode <Num-Of-Rounds> <Round-Duration>
```

Example:

```sh
./ex3_app Focus-Mode 2 5
```

During each round, you can type:
- `1` = Email notification (raises `SIGINT`)
- `2` = Reminder (raises `SIGUSR1`)
- `3` = Doorbell (raises `SIGUSR2`)
- `q` = end the current round early

The program blocks signals during the round, then checks which signals are pending and unblocks them.

### 2) CPU Scheduler

Run:

```sh
./ex3_app CPU-Scheduler <Processes.csv> <Time-Quantum>
```

Example (using the provided input):

```sh
./ex3_app CPU-Scheduler CPU-Scheduler/tester.csv 2
```

The scheduler simulates multiple algorithms (including Round Robin, which uses `<Time-Quantum>`).

## `tester.csv` format

Each line is:

```text
Name,Description,ArrivalTime,BurstTime,Priority
```

Example:

```text
P1,Initial system setup,0,5,3
```
