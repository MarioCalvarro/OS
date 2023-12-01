#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define M 10

int finish = 0;

void handler(int signo)
{
    printf("Recibida la señal %d.\n", signo);
    finish = 1;
}

void cocina(int* raciones, sem_t* cocinero, sem_t* salvajes)
{
    while(!finish) {
        if (sem_wait(cocinero) == -1) {
            perror("Error en el wait cocinero\n");
            return;
        }
        printf("El cocinero pone %d raciones.\n", M);
        *raciones = M;
        sem_post(salvajes);
    }
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_flags = 0; //Si el proceso recibe una señal en el bloqueo, se tira una excepción (lo que queremos)
    //act.sa_flags = SA_RESTART; //Si el proceso recibe una señan el el wait, se procesa la señal y, al volver, sigue donde estaba "bloqueado"
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);

    int shd = shm_open("RACIONES", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shd, sizeof(int));
    int* raciones = (int*)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shd, 0);
    *raciones = M; //Raciones iniciales

    sem_t* cocinero = sem_open("COCINERO", O_CREAT, 0755, 0);
    sem_t* salvajes = sem_open("SALVAJES", O_CREAT, 0755, 0);
    sem_t* caldero = sem_open("CALDERO", O_CREAT, 0755, 1); //Sección crítica

    cocina(raciones, cocinero, salvajes);

    munmap(raciones, sizeof(int));
    close(shd);
    shm_unlink("RACIONES");
    sem_close(cocinero);
    sem_unlink("COCINERO");
    sem_close(salvajes);
    sem_unlink("SALVAJES");
    sem_close(caldero);
    sem_unlink("CALDERO");

	return 0;
}
