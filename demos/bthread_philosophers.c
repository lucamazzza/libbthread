#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bthread.h"
#include "bthread_private.h"
#include "tmutex.h"
#include "tsemaphore.h"

#define N           5
#define LEFT(i)     ((i + N - 1) % N)
#define RIGHT(i)    ((i + 1) % N)
#define THINKING    0
#define HUNGRY      1
#define EATING      2

int state[N];
bthread_mutex_t mutex;
bthread_sem_t sem[N];

void *philosopher(void* arg);
void  take_forks(int i);
void  put_forks(int i);
void  test(int i);
void  think(int i);
void  eat(int i);

void think(int i) {
    bthread_printf("Philosopher %d is thinking...\n", i);
    bthread_sleep(200);
}

void eat(int i) {
    bthread_printf("Philosopher %d is eating...\n", i);
    bthread_sleep(300);
}

void *philosopher(void* arg) {
    volatile int i;
    i = (intptr_t)arg;
    while (1) {
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
    bthread_printf("\tPhilosopher %d DEAD\n", i);
}

void take_forks(int i) {
    bthread_mutex_lock(&mutex);
    state[i] = HUNGRY;
    bthread_printf("Philosopher %d is HUNGRY\n", i);
    test(i);
    bthread_mutex_unlock(&mutex);
    bthread_sem_wait(&sem[i]);
}

void put_forks(int i) {
    bthread_mutex_lock(&mutex);
    state[i] = THINKING;
    bthread_printf("Philosopher %d is THINKING\n", i);
    test(LEFT(i));
    test(RIGHT(i));
    bthread_mutex_unlock(&mutex);
}

void test(int i) {
    if (state[i] == HUNGRY && state[LEFT(i)] != EATING && state[RIGHT(i)] != EATING) {
        state[i] = EATING;
        bthread_printf("Philosopher %d is EATING\n", i);
        bthread_sem_post(&sem[i]);
    }
}

int main() {
    volatile bthread_t philosophers[N];
    for (int i = 0; i < N; i++) {
        bthread_sem_init(&sem[i], 0, 0);
        state[i] = THINKING;
    }
    bthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < N; i++) {
        bthread_create(&philosophers[i], NULL, philosopher, (void *)(intptr_t)i);
    }
    for (int i = 0; i < N; i++) {
        bthread_join(philosophers[i], NULL);
    }
    for (int i = 0; i < N; i++) {
        bthread_sem_destroy(&sem[i]);
    }
    bthread_mutex_destroy(&mutex);
    return 0;
}
