#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3
#define ROUNDS 2

// Shared counter accessed by all threads
int counter = 0;

// Mutex to protect the shared counter (prevents race conditions)
pthread_mutex_t counter_lock;

// Variables used to simulate Round-Robin scheduling
int turn = 0;
pthread_mutex_t turn_lock;
pthread_cond_t turn_cv;

// Two resources used to demonstrate deadlock prevention
pthread_mutex_t resA, resB;

// Function executed by each thread
void *task(void *arg)
{
    int id = *(int *)arg;

    for (int r = 1; r <= ROUNDS; r++)
    {
        // Wait until the scheduler gives this thread its turn
        pthread_mutex_lock(&turn_lock);
        while (turn != id)
            pthread_cond_wait(&turn_cv, &turn_lock);

        printf("Round %d -> Thread %d is scheduled (running)\n", r, id);

        // Protect the shared counter using a mutex
        pthread_mutex_lock(&counter_lock);
        counter++;
        pthread_mutex_unlock(&counter_lock);

        // Lock resources in a fixed order to prevent deadlock
        pthread_mutex_lock(&resA);
        pthread_mutex_lock(&resB);

        printf("           Thread %d used Resource A and Resource B safely (no deadlock)\n", id);

        pthread_mutex_unlock(&resB);
        pthread_mutex_unlock(&resA);

        // Simulate a CPU time slice before the next thread runs
        usleep(200000);

        // Pass execution to the next thread (Round-Robin scheduling)
        turn = (turn + 1) % NUM_THREADS;

        // Wake up waiting threads to check whose turn is next
        pthread_cond_broadcast(&turn_cv);

        pthread_mutex_unlock(&turn_lock);
    }

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS] = {0, 1, 2};

    // Initialize mutexes and condition variable
    pthread_mutex_init(&counter_lock, NULL);
    pthread_mutex_init(&turn_lock, NULL);
    pthread_cond_init(&turn_cv, NULL);
    pthread_mutex_init(&resA, NULL);
    pthread_mutex_init(&resB, NULL);

    // Create worker threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, task, &ids[i]);
    }

    // Wait for all worker threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Display program summary
    printf("\nFinal Counter = %d (Expected = %d)\n", counter, NUM_THREADS * ROUNDS);
    printf("Race condition prevented using counter_lock.\n");
    printf("Round-Robin scheduling simulated using turn, mutex, and condition variable.\n");
    printf("Deadlock prevented using consistent lock ordering (Resource A -> Resource B).\n");

    // Destroy mutexes and condition variable
    pthread_mutex_destroy(&counter_lock);
    pthread_mutex_destroy(&turn_lock);
    pthread_cond_destroy(&turn_cv);
    pthread_mutex_destroy(&resA);
    pthread_mutex_destroy(&resB);

    return 0;
}
