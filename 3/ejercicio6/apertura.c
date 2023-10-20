#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char opt;
	while ((opt = getopt(argc, argv, "f:rw")) != -1) {
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
    return 0;
}
