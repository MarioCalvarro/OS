#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define NUMITER 30

void salvaje(int* raciones, sem_t* cocinero, sem_t* salvajes, sem_t* caldero)
{
    for (int i = 0; i < NUMITER; ++i) {
        sem_wait(caldero);
        if (*raciones == 0) {
            sem_post(cocinero); 
            sem_wait(salvajes);
        }
        int seconds = 1 + rand() % 5;
        printf("Salvaje %lu comiendo durante %d segundos, quedan %d porciones\n", (long unsigned int)getpid(), seconds, --(*raciones));
        fflush(stdout);
        sem_post(caldero);
        sleep(seconds);
    }
}

int main(int argc, char *argv[])
{
    int shd = shm_open("RACIONES", O_RDWR, S_IRUSR);
    int* raciones = (int*) mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shd, 0);
    sem_t* cocinero = sem_open("COCINERO", 0);
    sem_t* salvajes = sem_open("SALVAJES", 0);
    sem_t* caldero = sem_open("CALDERO", 0);

    salvaje(raciones, cocinero, salvajes, caldero);

    munmap(raciones, sizeof(int));
    close(shd);
    sem_close(cocinero);
    sem_close(salvajes);
    sem_close(caldero);

	return 0;
}
