#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#define M 10

int finish = 0;

void cocina(int* raciones, sem_t* cocinero, sem_t* salvajes)
{
    while(!finish) {
        sem_wait(cocinero);
        *raciones = M;
        sem_post(salvajes);
    }
}

int main(int argc, char *argv[])
{
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
