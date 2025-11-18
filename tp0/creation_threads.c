#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* fonc(void* arg) {
    int i;
    int task_id = *((int*)arg); 
    for (i = 0; i < 7; i++) {
        printf("Tache %d : %d\n", task_id, i);
        usleep(1000000); 
    }
    free(arg); 
    return NULL;
}

int main(void) {
    pthread_t tache1, tache2;
    int* arg1 = malloc(sizeof(int)); 
    int* arg2 = malloc(sizeof(int)); 
    *arg1 = 1; 
    *arg2 = 2; 
    pthread_create(&tache1, NULL, fonc, arg1);
    pthread_create(&tache2, NULL, fonc, arg2);
    pthread_join(tache1, NULL);
    pthread_join(tache2, NULL);
    return 0;
}






