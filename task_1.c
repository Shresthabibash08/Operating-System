#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Shared counter that multiple threads increment; protected by 'lock'
int counter = 0;

// Mutex used to protect 'counter' from concurrent access (race condition fix)
pthread_mutex_t lock;

// Two "shared resources" represented as mutexes.
// Both threadOne and threadTwo lock them in the SAME order (resource1 then
// resource2), which prevents circular-wait deadlock.
pthread_mutex_t resource1;
pthread_mutex_t resource2;

void *threadOne(void *arg)
{
    printf("Thread 1 is running\n");

    // Critical section: protect 'counter' from race condition
    pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);
    // End critical section

    // Lock resources in same order as Thread 2 (resource1 -> resource2)
    // to prevent deadlock (no circular wait possible).
    pthread_mutex_lock(&resource1);
    pthread_mutex_lock(&resource2);

    printf("Thread 1 is using shared resources\n");
    sleep(1); // Simulate work being done while holding both resources

    // Release resources in reverse order of acquisition
    pthread_mutex_unlock(&resource2);
    pthread_mutex_unlock(&resource1);

    return NULL;
}

void *threadTwo(void *arg)
{
    printf("Thread 2 is running\n");

    // Critical section: protect 'counter' from race condition
    pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);
    // End critical section

    // Same lock order as Thread 1 -> prevents deadlock
    pthread_mutex_lock(&resource1);
    pthread_mutex_lock(&resource2);

    printf("Thread 2 is using shared resources\n");
    sleep(1); // Simulate work being done while holding both resources

    pthread_mutex_unlock(&resource2);
    pthread_mutex_unlock(&resource1);

    return NULL;
}


// Thread 3: simulates Round Robin CPU scheduling for 3 processes
// (P1, P2, P3) with a fixed time quantum. 
void *threadThree(void *arg)
{
    int burstTime[] = {5, 7, 3}; // Remaining CPU burst time for P1, P2, P3
    int quantum = 2;             // Fixed time slice given to each process
    int finished = 0;            // Count of processes that have completed

    printf("\nRound Robin Scheduling\n");

    // Keep looping through the processes until all 3 have finished
    while (finished < 3)
    {
        finished = 0; // Recount finished processes each full pass

        for (int i = 0; i < 3; i++)
        {
            if (burstTime[i] > 0)
            {
                printf("Process P%d is running\n", i + 1);

                if (burstTime[i] > quantum)
                {
                    // Process needs more time than one quantum:
                    // run for 'quantum' units and requeue (next loop pass)
                    burstTime[i] -= quantum;
                    printf("Remaining Time = %d\n", burstTime[i]);
                }
                else
                {
                    // Remaining time fits within a quantum: finish it now
                    burstTime[i] = 0;
                    printf("Process P%d completed\n", i + 1);
                }

                sleep(1); // Simulate the passage of time while process runs
            }

            // If this process has no time left, count it as finished
            if (burstTime[i] == 0)
                finished++;
        }
    }

    return NULL;
}

int main()
{
    pthread_t t1, t2, t3;

    // Initialize all mutexes before any thread uses them
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&resource1, NULL);
    pthread_mutex_init(&resource2, NULL);

    // Launch all three threads to run concurrently
    pthread_create(&t1, NULL, threadOne, NULL);
    pthread_create(&t2, NULL, threadTwo, NULL);
    pthread_create(&t3, NULL, threadThree, NULL);

    // Wait for all threads to finish before proceeding
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    // Because 'counter' was protected with a mutex, this will always be 2,
    // regardless of thread interleaving.
    printf("\nFinal Counter = %d\n", counter);
    printf("Race condition prevented using mutex.\n");
    printf("Deadlock prevented by locking resources in the same order.\n");

    // Clean up mutex resources
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&resource1);
    pthread_mutex_destroy(&resource2);

    return 0;
}
