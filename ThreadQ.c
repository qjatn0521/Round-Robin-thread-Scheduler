#include "ThreadQ.h"
#include <signal.h>
#include <stdlib.h>
ThreadQueue *readyQueue;
ThreadQueue *waitQueue;
Thread *currentThread;
int isEmpty(ThreadQueue *queue) { return (queue->head == NULL); }
void enqueue(ThreadQueue *queue, Thread *data) {
    if (isEmpty(queue)) {
        queue->head = data;
        queue->tail = data;
    } else {
        data->pPrev = queue->tail;
        queue->tail->pNext = data;
        queue->tail = data;
    }
    if (queue == readyQueue) {
        ReadyQHead = queue->head;
        ReadyQTail = queue->tail;
    } else {
        WaitQHead = queue->head;
        WaitQTail = queue->tail;
    }
}
Thread *dequeue(ThreadQueue *queue) {
    if (isEmpty(queue)) {
        return NULL;
    }
    Thread *tmp = queue->head;

    queue->head = queue->head->pNext;
    tmp->pNext = NULL;
    if (queue == readyQueue) {
        ReadyQHead = queue->head;
        ReadyQTail = queue->tail;
    } else {
        WaitQHead = queue->head;
        WaitQTail = queue->tail;
    }
    return tmp;
}
void __thread_to_ready(int signo) {
    Thread *pTh;
    pTh = getCurrentThread();
    pthread_mutex_lock(&pTh->readyMutex);

    pTh->status = THREAD_STATUS_READY;
    pTh->bRunnable = 0;

    enqueue(readyQueue, pTh);

    pthread_cond_signal(&pTh->readyCond);
    pthread_mutex_unlock(&pTh->readyMutex);

    pthread_mutex_lock(&pTh->readyMutex);
    while (pTh->bRunnable == 0) {
        pthread_cond_wait(&pTh->readyCond, &pTh->readyMutex);
    }
    pthread_mutex_unlock(&pTh->readyMutex);
}
void __thread_to_run(Thread *thread) {
    pthread_mutex_lock(&thread->readyMutex);
    thread->status = THREAD_STATUS_RUN;
    thread->bRunnable = 1;

    pthread_cond_signal(&thread->readyCond);
    currentThread = thread;
    pthread_mutex_unlock(&thread->readyMutex);
}

void threadInit() {
    signal(SIGUSR1, __thread_to_ready);
    readyQueue = (ThreadQueue *)malloc(sizeof(ThreadQueue));
    readyQueue->head = NULL;
    readyQueue->tail = NULL;
    waitQueue = (ThreadQueue *)malloc(sizeof(ThreadQueue));
    waitQueue->head = NULL;
    waitQueue->tail = NULL;
}
Thread *getCurrentThread() { return currentThread; }
