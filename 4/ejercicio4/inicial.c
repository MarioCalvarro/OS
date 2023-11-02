#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    int fd1, fd2, pos;
    char c;
    char buffer[6];

    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    for (int i = 0; i < 9; i++) {
        if (fork() == 0) {
            /* Child */
            //5: Initial Pos
            //2: 2 blocks (00000 and iiiii)
            //5: Number of chars per block
            sprintf(buffer, "%d", (i+1)*11111);
            pos = lseek(fd1, 5 + 2*i*5*sizeof(char), SEEK_SET); 
            printf("i = %d, pos = %d\n", i+1, pos);
            lseek(fd1, pos, SEEK_SET);
            write(fd1, buffer, 5);
            close(fd1);
            exit(0);
        } else {
            /* Parent */
            pos = lseek(fd1, 2*i*5*sizeof(char), SEEK_SET);
            lseek(fd1, pos, SEEK_SET);
            write(fd1, "00000", 5);
        }
    }

	//wait for all childs to finish
    while (wait(NULL) != -1);

    lseek(fd1, 0, SEEK_SET);
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0)
        printf("%c", (char) c);
    printf("\n");
    close(fd1);
    exit(0);
}
