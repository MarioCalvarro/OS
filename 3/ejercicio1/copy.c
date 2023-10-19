#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define N_BYTES 512

void copy(int fdo, int fdd)
{
    char buff[N_BYTES];
    ssize_t bytes_read = 0;
    while ((bytes_read = read(fdo, buff, sizeof(buff))) > 0) {
        write(fdd, buff, bytes_read);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        perror("Incorrect number of arguments. 2 are required.");
        return 1;
    }
    
    //Input file (it must exist, read only) 
    int fdo = open(argv[1], O_RDONLY);
    if (fdo == -1) {
        perror("Could not open input file.");
        return 1;
    }

    //Output file (write only, create if non-existent, delete previous content
    //             read and write permissions for user)
    int fdd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fdd == -1) {
        perror("Could not open output file");
        return 1;
    }

    copy(fdo, fdd);

    close(fdo);
    close(fdd);

	return 0;
}
