#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "defs.h"

#define NUMBER_CHARS 4

int open_file(struct options options) 
{
    int fdo;
    if (options.read_file == 0 && options.write_file == 0) {
        perror("No read or write option selected.");
        exit(EXIT_FAILURE);
    } else if (options.write_file == 0) {
        fdo = open(options.input_file, O_RDONLY | O_CREAT | O_TRUNC);
    } else if (options.read_file == 0) {
        fdo = open(options.input_file, O_WRONLY | O_CREAT | O_TRUNC);
    } else {
        fdo = open(options.input_file, O_RDWR | O_CREAT | O_TRUNC);
    }

    return fdo;
}

int main(int argc, char *argv[]) {
    char opt;
    printf("Hola");

    struct options options;

    options.input_file = NULL;
    options.read_file = 0;
    options.write_file = 0;

	while ((opt = getopt(argc, argv, "f:rw")) != -1) {
    switch (opt) {
    case 'f':
        options.input_file = optarg;        
        break;
    case 'r':
        options.read_file = 1;
        break;
    case 'w':
        options.write_file = 1;
        break;
    default:
        exit(EXIT_FAILURE);
    }
    }

    if (options.input_file == NULL) {
        perror("No input file selected");
        exit(EXIT_FAILURE);
    }

    int fdo = open_file(options);

    //If less than 1 bytes has been read is because of
    // 0: Nothing read
    // -1: Error
    if (write(fdo, &"1234", NUMBER_CHARS) < 1) {
        perror("Could not write file.");
    }

    char buf[NUMBER_CHARS];
    if (read(fdo, buf, NUMBER_CHARS) < 1) {
        perror("Could not read file.");
    }

    close(fdo);
    return 0;
}
