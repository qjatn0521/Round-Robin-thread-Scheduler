#include "Scheduler.h"
#include "Init.h"
#include "Thread.h"
#include "ThreadQ.h"
#include <signal.h>

int RunScheduler(void) {
    ThreadQueue *q = getReadyQ();
    while (1) {
        if (isEmpty(q)) {
        } else {
            Thread *nextThread = q->head;
            dequeue(q);
            if (getCurrentThread() != NULL) {
                Thread *curThread = getCurrentThread();
                if (curThread->bZombie == 1) {
                    enqueue(q, getCurrentThread());
                } else {
                    pthread_mutex_lock(&curThread->readyMutex);
                    pthread_kill(curThread->tid, SIGUSR1);

                    while (curThread->bRunnable == 1) {
                        pthread_cond_wait(&curThread->readyCond,
                                          &curThread->readyMutex);
                    }
                    pthread_mutex_unlock(&curThread->readyMutex);
                }
            }
            __thread_to_run(nextThread);
        }

        sleep(TIMESLICE);
    }
}

void __ContextSwitch(Thread *pCurThread, Thread *pNewThread) {}
