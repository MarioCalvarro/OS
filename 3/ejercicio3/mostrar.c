#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

void show_file(char* file_name, int N, char from_end)
{    
    //Input file (it must exist, read only) 
    int fdo = open(file_name, O_RDONLY);
    if (fdo == -1) {
        perror("Could not open input file.");
        return;
    }

    if (from_end) {
        lseek(fdo, -N, SEEK_END);
    } 
    else {
        lseek(fdo, N, SEEK_SET);
    }

    char buff[1];
	/* Read file byte by byte */
    while (read(fdo, buff, sizeof(char)) > 0) {
		/* Print byte to stdout */
        write(1, buff, sizeof(char));
    }

    return;
}

int main(int argc, char *argv[])
{
    char opt;
    int N = 0;
    char from_end = 0;

	while ((opt = getopt(argc, argv, "n:e")) != -1) {
    switch (opt) {
    case 'n':
        N = atoi(optarg); 
        break;
    case 'e':
        from_end = 1;
        break;
    default:
        exit(EXIT_FAILURE);
    }
    }

    char* file_name = argv[optind];
    show_file(file_name, N, from_end);
    
	return 0;
}
