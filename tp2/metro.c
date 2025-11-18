#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t seg_AB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t seg_BC = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t seg_CD = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t seg_FB = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t seg_CE = PTHREAD_MUTEX_INITIALIZER;  

void traverser(const char* metro, int id, pthread_mutex_t* mutex, const char* segment) {
    pthread_mutex_lock(mutex);
    printf("[%.0f] %s-%d  → occupe %s\n", (double)time(NULL), metro, id, segment);
    sleep(2);  
    pthread_mutex_unlock(mutex);
    printf("[%.0f] %s-%d  ← libère %s\n", (double)time(NULL), metro, id, segment);
}

// MR1 : A → B → C → D
void* metro_MR1(void* arg) {
    int id = *(int*)arg;
    while (1) {
        printf("[%.0f] MR1-%d départ A → D\n", (double)time(NULL), id);

        traverser("MR1", id, &seg_AB, "A→B");
        traverser("MR1", id, &seg_BC, "B→C");
        traverser("MR1", id, &seg_CD, "C→D");

        printf("[%.0f] MR1-%d arrivé à D, demi-tour\n", (double)time(NULL), id);
        sleep(2);

        traverser("MR1", id, &seg_CD, "D→C");
        traverser("MR1", id, &seg_BC, "C→B");
        traverser("MR1", id, &seg_AB, "B→A");

        printf("[%.0f] MR1-%d revenu à A, pause\n", (double)time(NULL), id);
        sleep(4);
    }
    return NULL;
}

// MR2 : F → B → A 
void* metro_MR2(void* arg) {
    int id = *(int*)arg;
    while (1) {
        printf("[%.0f] MR2-%d départ F → A\n", (double)time(NULL), id);

        traverser("MR2", id, &seg_FB, "F→B");
        traverser("MR2", id, &seg_AB, "B→A");   

        printf("[%.0f] MR2-%d arrivé à A, demi-tour\n", (double)time(NULL), id);
        sleep(2);

        traverser("MR2", id, &seg_AB, "A→B");
        traverser("MR2", id, &seg_FB, "B→F");

        printf("[%.0f] MR2-%d revenu à F, pause\n", (double)time(NULL), id);
        sleep(5);
    }
    return NULL;
}

// MR3 : E → C → B  
void* metro_MR3(void* arg) {
    int id = *(int*)arg;
    while (1) {
        printf("[%.0f] MR3-%d départ E → B\n", (double)time(NULL), id);

        traverser("MR3", id, &seg_CE, "E→C");
        traverser("MR3", id, &seg_BC, "C→B");

        printf("[%.0f] MR3-%d arrivé à B, demi-tour\n", (double)time(NULL), id);
        sleep(2);

        traverser("MR3", id, &seg_BC, "B→C");
        traverser("MR3", id, &seg_CE, "C→E");

        printf("[%.0f] MR3-%d revenu à E, pause\n", (double)time(NULL), id);
        sleep(6);
    }
    return NULL;
}

int main() {
    pthread_t t[9];
    int ids[9] = {1,2,3,4,5,6,7,8,9};

    // 3 métros par ligne
    pthread_create(&t[0], NULL, metro_MR1, &ids[0]);
    pthread_create(&t[1], NULL, metro_MR1, &ids[1]);
    pthread_create(&t[2], NULL, metro_MR1, &ids[2]);

    pthread_create(&t[3], NULL, metro_MR2, &ids[3]);
    pthread_create(&t[4], NULL, metro_MR2, &ids[4]);

    pthread_create(&t[5], NULL, metro_MR3, &ids[5]);
    pthread_create(&t[6], NULL, metro_MR3, &ids[6]);
    pthread_create(&t[7], NULL, metro_MR3, &ids[7]);

    sleep(80);  
    printf("=== FIN DE LA SIMULATION ===\n");
    return 0;
}