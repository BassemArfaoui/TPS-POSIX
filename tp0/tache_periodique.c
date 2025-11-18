

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <linux/time.h>

void* tachePeridique (void* periode) {
    pthread_cond_t cond;
    pthread_mutex_t verrou;
    struct timespec time;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&verrou, NULL);
    int i = 0;
    clock_gettime (CLOCK_REALTIME, &time);
    while(i < 10) {
        pthread_mutex_lock(&verrou);
        time.tv_sec = time.tv_sec + (int) periode;
        printf("La tache %s s'execute periodiquement à l'instant %ld secondes\n", "t1", time.tv_sec);
        pthread_mutex_unlock(&verrou);
        i++;
    }
    return NULL;
}


//periode 6s
int main(void) {
    pthread_t tache1;
    pthread_create(&tache1, NULL, tachePeridique, (void*)6);
 
    pthread_join (tache1, NULL);
    return 0;
}