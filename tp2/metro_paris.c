

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_STATIONS_PER_ROUTE 32
#define MAX_SEGMENTS 256
#define MAX_TRAINS 64

typedef struct {
    char *name;
    int nstations;
    char *stations[MAX_STATIONS_PER_ROUTE];
} route_t;

typedef struct {
    char key[16];
    pthread_mutex_t mutex;
    int id;
} segment_t;

segment_t segments[MAX_SEGMENTS];
int nsegments = 0;

typedef struct {
    int id;
    char *line_name;
    int nsegments_route;
    int seg_ids[MAX_STATIONS_PER_ROUTE];
    int delay_start;
} train_args_t;


//Segment management


int get_or_add_segment(const char *from, const char *to) {
    char key[16];
    snprintf(key, sizeof(key), "%s-%s", from, to);

    for (int i = 0; i < nsegments; ++i) {
        if (strcmp(segments[i].key, key) == 0)
            return segments[i].id;
    }

    if (nsegments >= MAX_SEGMENTS) {
        fprintf(stderr, "Too many segments\n");
        exit(1);
    }

    int id = nsegments;

    strncpy(segments[nsegments].key, key, sizeof(segments[nsegments].key)-1);
    segments[nsegments].key[sizeof(segments[nsegments].key)-1] = '\0';

    pthread_mutex_init(&segments[nsegments].mutex, NULL);
    segments[nsegments].id = id;

    nsegments++;
    return id;
}

int build_route_segments(route_t *r, int seg_ids[]) {
    int cnt = 0;
    for (int i = 0; i < r->nstations - 1; ++i) {
        seg_ids[cnt++] = get_or_add_segment(r->stations[i], r->stations[i+1]);
    }
    return cnt;
}



//Train thread


void print_time() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}



void *train_thread(void *arg) {
    train_args_t *tr = (train_args_t*)arg;

    if (tr->delay_start > 0)
        sleep(tr->delay_start);

    printf("["); print_time(); printf("] Train %d (%s) : départ\n",
           tr->id, tr->line_name);

    for (int i = 0; i < tr->nsegments_route; ++i) {
        int segid = tr->seg_ids[i];

        printf("["); print_time(); printf("] Train %d (%s) demande %s\n",
               tr->id, tr->line_name, segments[segid].key);

        pthread_mutex_lock(&segments[segid].mutex);

        printf("["); print_time(); printf("] Train %d (%s) OCCUPE %s (2s)\n",
               tr->id, tr->line_name, segments[segid].key);

        sleep(2);

        pthread_mutex_unlock(&segments[segid].mutex);

        printf("["); print_time(); printf("] Train %d (%s) libère %s\n",
               tr->id, tr->line_name, segments[segid].key);

        usleep((rand() % 500 + 200) * 1000);
    }

    printf("["); print_time(); printf("] Train %d (%s) FIN DE PARCOURS\n",
           tr->id, tr->line_name);

    free(tr);
    return NULL;
}


//create_trains_for_route


void create_trains_for_route(
    route_t *r,
    int seg_ids[],
    int nseg,
    int ntrains,
    int start_id_base,
    pthread_t threads[],
    int *tcount
) {
    for (int k = 0; k < ntrains; ++k) {

        train_args_t *arg = malloc(sizeof(train_args_t));
        arg->id = start_id_base + k;
        arg->line_name = r->name;
        arg->nsegments_route = nseg;

        for (int i = 0; i < nseg; ++i)
            arg->seg_ids[i] = seg_ids[i];

        arg->delay_start = rand() % 3;

        if (pthread_create(&threads[*tcount], NULL, train_thread, arg) != 0) {
            perror("pthread_create");
            exit(1);
        }

        (*tcount)++;
        usleep((rand() % 400 + 100) * 1000);
    }
}

//MAIN

int main() {
    srand(time(NULL));

    //build the schema
    route_t verte = {
        .name = "Verte",
        .nstations = 7,
        .stations = {"A","P","L","G","M","N","O"}
    };
    route_t marron = {
        .name = "Marron",
        .nstations = 2,
        .stations = {"D","K"}
    };
    route_t bleue = {
        .name = "Bleue",
        .nstations = 6,
        .stations = {"B","C","I","R","S","T"}
    };
    route_t rouge = {
        .name = "Rouge",
        .nstations = 6,
        .stations = {"E","F","G","H","I","J"}
    };
    route_t noire = {
        .name = "Noire",
        .nstations = 2,
        .stations = {"Q","U"}   
    };
    /* Segments */
    int sv[MAX_STATIONS_PER_ROUTE];
    int sm[MAX_STATIONS_PER_ROUTE];
    int sb[MAX_STATIONS_PER_ROUTE];
    int sr[MAX_STATIONS_PER_ROUTE];
    int sn[MAX_STATIONS_PER_ROUTE];

    int nsv = build_route_segments(&verte, sv);
    int nsm = build_route_segments(&marron, sm);
    int nsb = build_route_segments(&bleue, sb);
    int nsr = build_route_segments(&rouge, sr);
    int nsn = build_route_segments(&noire, sn);

    //print 
    printf("Segments créés :\n");
    for (int i = 0; i < nsegments; ++i) {
        printf("  id=%d -> %s\n", segments[i].id, segments[i].key);
    }
    pthread_t threads[MAX_TRAINS];
    int tcount = 0;

    /* Create trains */
    create_trains_for_route(&verte, sv, nsv, 3, 100, threads, &tcount);
    create_trains_for_route(&marron, sm, nsm, 2, 200, threads, &tcount);
    create_trains_for_route(&bleue, sb, nsb, 3, 300, threads, &tcount);
    create_trains_for_route(&rouge, sr, nsr, 2, 400, threads, &tcount);
    create_trains_for_route(&noire, sn, nsn, 1, 500, threads, &tcount);

    /* Join all */
    for (int i = 0; i < tcount; ++i)
        pthread_join(threads[i], NULL);

    printf("Tous les trains ont terminé.\n");

    for (int i = 0; i < nsegments; ++i)
        pthread_mutex_destroy(&segments[i].mutex);

    return 0;
}
