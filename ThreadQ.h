#ifndef __THREADQ_H__
#define __THREADQ_H__
#include "Thread.h"
typedef void *(*FuncPtr)(void *);

typedef struct __ThreadQueue {
    Thread *head;
    Thread *tail;
} ThreadQueue;
extern ThreadQueue *readyQueue;
extern ThreadQueue *waitQueue;
extern Thread *currentThread;
int isEmpty(ThreadQueue *queue);
void enqueue(ThreadQueue *queue, Thread *data);
Thread *dequeue(ThreadQueue *queue);
void __thread_to_ready(int signo);
void __thread_to_run(Thread *thread);
void initQueue();

ThreadQueue *getReadyQ();
ThreadQueue *getWaitQ();

Thread *getCurrentThread();
void threadInit();
#endif
