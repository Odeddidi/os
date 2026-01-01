//oded didi 211913587
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

sigset_t blockSet;

void emaiHandler(int sigNum) {
    printf("[Outcome:] The TA announced: Everyone get 100 on the exercise!\n");
}
void remiderHandler(int sigNum) {
    printf("[Outcome:] You picked it up just in time.\n");
}
void doorBellHandler(int sigNum) {
    printf("[Outcome:] Food delivery is here.\n");
}

void printDisplay() {
    printf("\n");
    printf("Simulate a distraction:\n");
    printf("  1 = Email notification\n");
    printf("  2 = Reminder to pick up delivery\n");
    printf("  3 = Doorbell Ringing\n");
    printf("  q = Quit\n");
    printf(">> ");

}
void runFocusMode(int numOfRounds, int duration) {
    // Set up signal handlers
    struct sigaction email;
    email.sa_handler = emaiHandler;
    sigemptyset(&email.sa_mask);
    email.sa_flags = 0;
    sigaction(SIGINT, &email, NULL);
    struct sigaction reminder;
    reminder.sa_handler = remiderHandler;
    sigemptyset(&reminder.sa_mask);
    reminder.sa_flags = 0;
    sigaction(SIGUSR1, &reminder, NULL);
    struct sigaction doorBell;
    doorBell.sa_handler = doorBellHandler;
    sigemptyset(&doorBell.sa_mask);
    doorBell.sa_flags = 0;
    sigaction(SIGUSR2, &doorBell, NULL);
    
    printf("Entering Focus Mode. All distractions are blocked.\n");
    
    // Run the focus mode for the specified number of rounds
    for (int i = 0; i < numOfRounds; i++){
        printf("══════════════════════════════════════════════\n");
        printf("                Focus Round %d                \n", i + 1);
        printf("──────────────────────────────────────────────\n");
        // Block all signals
        sigfillset(&blockSet);
        sigprocmask(SIG_SETMASK, &blockSet, NULL);
        for (int j = 0; j < duration; j++) {
           printDisplay();

            //Get user input for distractions
            char key;
            scanf(" %c", &key);

            switch (key)
            {
                // 1 = Email notification (sig_int)
            case '1':
            raise(SIGINT);
                break;
                // 2 = Reminder to pick up your delivery (sig_usr1)
            case '2':
                raise(SIGUSR1);
                break;
                // 3 = Doorbell ringing (sig_usr2)
            case '3':
                raise(SIGUSR2);
                break;
                // q = Quit focus mode
            case 'q':  
                break;
            default:
                break;
            }
            if (key == 'q') {
                break;
            }
            
        }
        printf("──────────────────────────────────────────────\n");
        printf("        Checking pending distractions...      \n");
        printf("──────────────────────────────────────────────\n");
        // Check for pending signals
        sigset_t pendingSet;
        sigemptyset(&pendingSet);
        sigpending(&pendingSet);
        sigset_t unblockSet;
        sigemptyset(&unblockSet);
        //If no signals are pending, print a message
        if (!sigismember(&pendingSet, SIGINT) && 
            !sigismember(&pendingSet, SIGUSR1) && 
            !sigismember(&pendingSet, SIGUSR2)) {
            printf("No distractions reached you this round.\n");
        }
        // If there are pending signals, print the corresponding messages
        if (sigismember(&pendingSet, SIGINT)) {
            printf(" - Email notification is waiting.\n");
            sigaddset(&unblockSet, SIGINT);
            sigprocmask(SIG_UNBLOCK, &unblockSet, NULL);

        }
        if (sigismember(&pendingSet, SIGUSR1)) {
            printf(" - You have a reminder to pick up your delivery.\n");
            sigaddset(&unblockSet, SIGUSR1);
            sigprocmask(SIG_UNBLOCK, &unblockSet, NULL);
        }
        if (sigismember(&pendingSet, SIGUSR2)) {
            printf(" - The doorbell is ringing.\n");
            sigaddset(&unblockSet, SIGUSR2);
            sigprocmask(SIG_UNBLOCK, &unblockSet, NULL);

        }
        
        
        // Unblock signals to check for pending distractions
        sigprocmask(SIG_UNBLOCK, &blockSet, NULL);
        printf("──────────────────────────────────────────────\n");
        printf("             Back to Focus Mode.              \n");
        printf("══════════════════════════════════════════════\n");
    }
    // Final message after all rounds
    printf("\nFocus Mode complete. All distractions are now unblocked.\n");
}