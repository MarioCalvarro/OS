//Mario Calvarro, Beñat Pérez
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 7
#define N 30
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t myturn = PTHREAD_COND_INITIALIZER;

int clients_inside = 0;
int waiting_vip = 0;
int local_turn = 0;
int local_turn_vip = 0;
int global_turn = 0;
int global_turn_vip = 0;

typedef struct client_data {
    int id;
    int my_turn;
    int is_vip; //0 normal; 1 vip
} client_t;

void arrive_client(client_t* client)
{
    pthread_mutex_lock(&m);

    client->my_turn = (client->is_vip) ? local_turn_vip++ : local_turn++;
    printf("Client %d (%s) receives his turn (%d).\n", client->id, VIPSTR(client->is_vip), client->my_turn); 
    fflush(stdout);

    if(client->is_vip) {
        waiting_vip++;
        while(client->my_turn != global_turn_vip || clients_inside >= CAPACITY) {
            pthread_cond_wait(&myturn, &m);
        }
    } else {
        while(waiting_vip || client->my_turn != global_turn || clients_inside >= CAPACITY) {
            pthread_cond_wait(&myturn, &m);
        }
    }

    pthread_mutex_unlock(&m);
}

void enter_and_dance(client_t* client)
{
    pthread_mutex_lock(&m);

    clients_inside++;
    if (client->is_vip) {
        waiting_vip--;
        global_turn_vip++;
    } else {
        global_turn++;
    }
    int seconds = rand() % 6 + 3;
    printf("Client %d (%s) is inside and dancing for %d seconds.\n", client->id, VIPSTR(client->is_vip), client->my_turn);
    fflush(stdout);
    pthread_cond_broadcast(&myturn);
    pthread_mutex_unlock(&m);

    sleep(seconds);
}

void exit_client(client_t* client)
{
    pthread_mutex_lock(&m);

    clients_inside--;
    printf("Client %d (%s) leaves.\n", client->id, VIPSTR(client->is_vip));
    fflush(stdout);
    pthread_cond_broadcast(&myturn);

    pthread_mutex_unlock(&m);
}

void *client(void *arg)
{
    client_t* client = (client_t*) arg;
    arrive_client(client);
    enter_and_dance(client);
    exit_client(client);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        perror("Incorrect number of arguments!");
        exit(1);
    }

    FILE* file = NULL;

    if ((file = fopen(argv[1], "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    int num_clients = 0;
    fscanf(file, "%d", &num_clients);

    pthread_t th_clients[num_clients];
    client_t clients[num_clients];
    for (int i = 0; i < num_clients; i++) {
        int client_type;
        fscanf(file, "%d", &client_type);

        clients[i].id = i;
        clients[i].my_turn = -1;
        clients[i].is_vip = client_type;
        pthread_create(th_clients + i, NULL, client, (void*)(clients + i));
    }

    for (int i = 0; i < num_clients; i++) {
        pthread_join(th_clients[i], NULL);
    }

    pthread_cond_destroy(&myturn);
    pthread_mutex_destroy(&m);
    return 0;
}
