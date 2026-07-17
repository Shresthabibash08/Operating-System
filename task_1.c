#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int counter = 0;

// Mutex for race condition
pthread_mutex_t lock;

// Two shared resources
pthread_mutex_t resource1;
pthread_mutex_t resource2;

// Thread 1
void *threadOne(void *arg)
{
    printf("Thread 1 is running\n");

    // Prevent race condition
    pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);

    // Lock resources in same order (prevents deadlock)
    pthread_mutex_lock(&resource1);
    pthread_mutex_lock(&resource2);

    printf("Thread 1 is using shared resources\n");
    sleep(1);

    pthread_mutex_unlock(&resource2);
    pthread_mutex_unlock(&resource1);

    return NULL;
}

// Thread 2
void *threadTwo(void *arg)
{
    printf("Thread 2 is running\n");

    pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);

    pthread_mutex_lock(&resource1);
    pthread_mutex_lock(&resource2);

    printf("Thread 2 is using shared resources\n");
    sleep(1);

    pthread_mutex_unlock(&resource2);
    pthread_mutex_unlock(&resource1);

    return NULL;
}

// Thread 3 - Round Robin Simulation
void *threadThree(void *arg)
{
    int burstTime[] = {5, 7, 3};
    int quantum = 2;
    int finished = 0;

    printf("\nRound Robin Scheduling\n");

    while (finished < 3)
    {
        finished = 0;

        for (int i = 0; i < 3; i++)
        {
            if (burstTime[i] > 0)
            {
                printf("Process P%d is running\n", i + 1);

                if (burstTime[i] > quantum)
                {
                    burstTime[i] -= quantum;
                    printf("Remaining Time = %d\n", burstTime[i]);
                }
                else
                {
                    burstTime[i] = 0;
                    printf("Process P%d completed\n", i + 1);
                }

                sleep(1);
            }

            if (burstTime[i] == 0)
                finished++;
        }
    }

    return NULL;
}

int main()
{
    pthread_t t1, t2, t3;

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&resource1, NULL);
    pthread_mutex_init(&resource2, NULL);

    pthread_create(&t1, NULL, threadOne, NULL);
    pthread_create(&t2, NULL, threadTwo, NULL);
    pthread_create(&t3, NULL, threadThree, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("\nFinal Counter = %d\n", counter);
    printf("Race condition prevented using mutex.\n");
    printf("Deadlock prevented by locking resources in the same order.\n");

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&resource1);
    pthread_mutex_destroy(&resource2);

    return 0;
}
