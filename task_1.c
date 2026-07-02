#include <stdio.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t lock;

void *task(void *arg) {
    int id = *(int *)arg;

    pthread_mutex_lock(&lock);
    counter++;
    printf("Thread %d incremented counter to %d\n", id, counter);
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    pthread_t threads[3];
    int ids[3] = {1, 2, 3};

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < 3; i++)
        pthread_create(&threads[i], NULL, task, &ids[i]);

    for (int i = 0; i < 3; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&lock);

    printf("Final counter value: %d\n", counter);
    return 0;
}
