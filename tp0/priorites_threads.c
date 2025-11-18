#include <stdio.h>
#include <pthread.h>
#include <sched.h>

void* fonc(void* arg) {
    int i;
    for (i = 0; i < 7; i++) {
        printf("Tache %d : %d\n", (int)arg, i);
        usleep(1000000);
    }
    return NULL;
}

int main(void) {
    pthread_t tache1, tache2;
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    param.sched_priority = 12;
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&tache1, &attr, fonc, (void*)1);

    param.sched_priority = 7;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&tache2, &attr, fonc, (void*)2);

    pthread_join(tache1, NULL);
    pthread_join(tache2, NULL);
    pthread_attr_destroy(&attr);
    return 0;
}