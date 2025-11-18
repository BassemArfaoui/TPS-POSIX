#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Capacités
#define POIDS_CAMION 15
#define POIDS_VOITURE 5
#define CAPACITE_PONT 15

int charge_pont = 0;

// Synchro
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_camions = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_voitures = PTHREAD_COND_INITIALIZER;

// Cpts
int attente_camions = 0;
int attente_voitures = 0;


// MONITEUR
void acceder_pont(int type) {
    pthread_mutex_lock(&mutex);

    if (type == 1) {    
        attente_camions++;
        while (charge_pont + POIDS_CAMION > CAPACITE_PONT) {
            pthread_cond_wait(&cond_camions, &mutex);
        }
        attente_camions--;
        charge_pont += POIDS_CAMION;
        printf("Camion entre sur le pont. Charge = %d\n", charge_pont);
    }

    else {  
        attente_voitures++;
        while (
            charge_pont + POIDS_VOITURE > CAPACITE_PONT ||
            attente_camions > 0   
        ) {
            pthread_cond_wait(&cond_voitures, &mutex);
        }
        attente_voitures--;
        charge_pont += POIDS_VOITURE;
        printf("Voiture entre sur le pont. Charge = %d\n", charge_pont);
    }

    pthread_mutex_unlock(&mutex);
}

void liberer_pont(int type) {
    pthread_mutex_lock(&mutex);

    if (type == 1)
        charge_pont -= POIDS_CAMION;
    else
        charge_pont -= POIDS_VOITURE;

    printf("↩Vehicule sort. Charge = %d\n", charge_pont);

    if (attente_camions > 0 && charge_pont + POIDS_CAMION <= CAPACITE_PONT) {
        pthread_cond_signal(&cond_camions);
    }
    else if (attente_voitures > 0 && charge_pont + POIDS_VOITURE <= CAPACITE_PONT) {
        pthread_cond_signal(&cond_voitures);
    }

    pthread_mutex_unlock(&mutex);
}



// THREADS
void* camion(void *arg) {
    acceder_pont(1);
    sleep(2);
    liberer_pont(1);
    return NULL;
}

void* voiture(void *arg) {
    acceder_pont(2);
    sleep(1);
    liberer_pont(2);
    return NULL;
}



int main() {
    pthread_t th[10];

    pthread_create(&th[0], NULL, camion, NULL);
    pthread_create(&th[1], NULL, voiture, NULL);
    pthread_create(&th[2], NULL, voiture, NULL);
    pthread_create(&th[3], NULL, camion, NULL);
    pthread_create(&th[4], NULL, voiture, NULL);

    for (int i = 0; i < 5; i++)
        pthread_join(th[i], NULL);

    return 0;
}
