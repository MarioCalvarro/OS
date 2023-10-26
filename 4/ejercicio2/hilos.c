#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct thread_info {
    int thread_num;
    char priority;
};

void *thread_usuario(void *arg)
{
    struct thread_info *info = arg;
    pthread_t id = pthread_self();
    int num = info->thread_num;
    char priority = info->priority;

    free(arg);
    printf("Thread con id:%ld, número:%d, y prioridad:%c\n", 
            id, num, priority);

    pthread_exit(0);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        perror("Incorrect number of arguments (only and only 1 required, num of threads)");
        exit(1);
    }
    int num_threads = atoi(argv[1]);
    struct thread_info *tinfo[num_threads];
    pthread_t tids[num_threads];

    for (size_t tnum = 0; tnum < num_threads; tnum++) {
        tinfo[tnum] = (struct thread_info*) malloc(sizeof(struct thread_info));
        tinfo[tnum]->thread_num = tnum + 1;
        tinfo[tnum]->priority = (tnum % 2 == 0)? 'P' : 'I'; //Par, impar
        
        pthread_create(&tids[tnum], NULL, &thread_usuario, tinfo[tnum]);
    }

    for (size_t tnum = 0; tnum < num_threads; tnum++) {
        pthread_join(tids[tnum], NULL);
    }

	return 0;
}
