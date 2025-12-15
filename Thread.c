#include "Thread.h"
#include "Init.h"
#include "Scheduler.h"
#include "ThreadQ.h"
typedef void *(*FuncPtr)(void *);
Thread *ReadyQHead = NULL;
Thread *ReadyQTail = NULL;

Thread *WaitQHead = NULL;
Thread *WaitQTail = NULL;

void *__wrapperFunc(void *arg) {
    void *ret;
    WrapperArg *pArg = (WrapperArg *)arg;
    FuncPtr funcPtr = pArg->funcPtr;
    Thread *thread = pArg->pThread;
    void *funcArg = pArg->funcArg;
    pthread_mutex_lock(&thread->readyMutex);
    pthread_cond_signal(&thread->readyCond);
    enqueue(readyQueue, thread);
    pthread_mutex_unlock(&thread->readyMutex);

    pthread_mutex_lock(&thread->readyMutex);
    while (thread->bRunnable == 0) {
        pthread_cond_wait(&thread->readyCond, &thread->readyMutex);
    }
    pthread_mutex_unlock(&thread->readyMutex);
    ret = funcPtr(funcArg);

    return ret;
}

int thread_create(thread_t *thread, thread_attr_t *attr,
                  void *(*start_routine)(void *), void *arg) {
    Thread *newThread = (Thread *)malloc(sizeof(Thread));
    if (newThread == NULL) {
        return -1;
    }
    newThread->status = THREAD_STATUS_READY;
    newThread->pExitCode = NULL;
    newThread->bRunnable = 0;
    newThread->pNext = NULL;
    newThread->pPrev = NULL;
    newThread->parentTid = thread_self();

    WrapperArg wrapperArg;
    wrapperArg.funcPtr = start_routine;
    wrapperArg.funcArg = arg;
    wrapperArg.pThread = newThread;
    newThread->status = THREAD_STATUS_READY;
    pthread_cond_init(&newThread->readyCond, attr);
    pthread_mutex_init(&newThread->readyMutex, attr);
    pthread_cond_init(&newThread->zombieCond, attr);
    pthread_mutex_init(&newThread->zombieMutex, attr);

    pthread_create(&newThread->tid, attr, __wrapperFunc, &wrapperArg);

    pthread_mutex_lock(&newThread->readyMutex);
    pthread_cond_wait(&newThread->readyCond, &newThread->readyMutex);

    pthread_mutex_unlock(&newThread->readyMutex);

    *thread = newThread->tid;
    return 0;
}

Thread *thread_find(ThreadQueue *q, thread_t tid) {
    Thread *tmp = q->head;
    while (tmp != NULL && tmp->tid != tid) {
        tmp = tmp->pNext;
    }
    if (tmp != NULL) {
        return tmp;
    } else
        return NULL;
}
int thread_join(thread_t thread, void **retval) {
    Thread *tmp = thread_find(readyQueue, thread);

    pthread_mutex_lock(&tmp->zombieMutex);
    while (tmp->bZombie == 0) {
        pthread_cond_wait(&tmp->zombieCond, &tmp->zombieMutex);
    }

    Thread *tmp2 = readyQueue->head;

    if (tmp->tid == readyQueue->head->tid) {
        dequeue(readyQueue);
    } else if (tmp->tid == readyQueue->tail->tid) {
        tmp->pPrev->pNext = NULL;
        readyQueue->tail = tmp->pPrev;
    } else {
        tmp->pPrev->pNext = tmp->pNext;
        tmp->pNext->pPrev = tmp->pPrev;
    }
    pthread_mutex_unlock(&tmp->zombieMutex);
    *retval = tmp->pExitCode;
}
int thread_exit(void *retval) {
    Thread *curThread = getCurrentThread();

    pthread_mutex_lock(&curThread->zombieMutex);
    curThread->bZombie = 1;
    curThread->pExitCode = retval;
    pthread_cond_signal(&curThread->zombieCond);
    pthread_mutex_unlock(&curThread->zombieMutex);
}

int thread_suspend(thread_t tid) {

    if (currentThread->tid == tid) {
        Thread *thread = getCurrentThread();
        thread->status = THREAD_STATUS_BLOCKED;
        enqueue(waitQueue, thread);
    } else {
        Thread *thread = thread_find(readyQueue, tid);
        thread->status = THREAD_STATUS_BLOCKED;
        if (thread->tid == readyQueue->head->tid) {
            dequeue(readyQueue);
        } else if (thread->tid == readyQueue->tail->tid) {
            thread->pPrev->pNext = NULL;
            readyQueue->tail = thread->pPrev;
            ReadyQTail = readyQueue->tail;
        } else {
            thread->pPrev->pNext = thread->pNext;
            thread->pNext->pPrev = thread->pPrev;
        }
        enqueue(waitQueue, thread);
    }
}

int thread_resume(thread_t tid) {
    Thread *thread = thread_find(waitQueue, tid);
    thread->status = THREAD_STATUS_READY;
    if (thread->tid == waitQueue->head->tid) {
        dequeue(waitQueue);
    } else if (thread->tid == waitQueue->tail->tid) {
        thread->pPrev->pNext = NULL;
        waitQueue->tail = thread->pPrev;
        WaitQTail = waitQueue->tail;
    } else {
        thread->pPrev->pNext = thread->pNext;
        thread->pNext->pPrev = thread->pPrev;
    }
    enqueue(readyQueue, thread);
}

thread_t thread_self() { return pthread_self(); }

ThreadQueue *getReadyQ() { return readyQueue; }
ThreadQueue *getWaitQ() { return waitQueue; }
