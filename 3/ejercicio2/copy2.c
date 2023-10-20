/** When we use the copy() from the previous exercise on a soft link we directly
 * copy the contents of the file it points to. This way the new file is not a 
 * soft link itself, simply a copy of the file pointed by the link which is 
 * being copied.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "defs.h"

#define N_BYTES 512

void copy(int fdo, int fdd)
{
    char buff[N_BYTES];
    ssize_t bytes_read = 0;
    while ((bytes_read = read(fdo, buff, sizeof(buff))) > 0) {
        write(fdd, buff, bytes_read);
    }
}

void copy_regular(char *orig, char *dest)
{
    //Input file (it must exist, read only) 
    int fdo = open(orig, O_RDONLY);
    if (fdo == -1) {
        perror("Could not open input file.");
        return;
    }

    //Output file (write only, create if non-existent, delete previous content
    //             read and write permissions for user)
    int fdd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fdd == -1) {
        perror("Could not open output file");
        return;
    }

    copy(fdo, fdd);

    close(fdo);
    close(fdd);

    return;
}

void copy_link(char *orig, char *dest)
{
    struct stat file_des;

    if (lstat(orig, &file_des) != 0) { 
        perror("File to read input file.");
        exit(1);
    }

    int file_size = (int)file_des.st_size + 1;

    char* buf = malloc(file_size);

    if (readlink(orig, buf, file_size) == -1) {
        perror("Failed to read symbolic link contents.");
        exit(1);
    }
    buf[file_size - 1] = '\0';

    if (symlink(buf, dest)) {
        perror("Failed to create new cloned soft link.");
        exit(1);
    }

    return;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        perror("Incorrect number of arguments. 2 are required.");
        return 1;
    }

    struct stat input_stat;
    file_t file_type = NORMAL_FILE;

    if (lstat(argv[1], &input_stat) == -1) {
        perror("File to read input file.");
        return 1;
    }

    if ((input_stat.st_mode & S_IFMT) == S_IFLNK) {
        file_type = SOFT_LINK_FILE;
    } 

    switch (file_type) {
    case NORMAL_FILE: 
        copy_regular(argv[1], argv[2]);
        break;
    case SOFT_LINK_FILE:
        copy_link(argv[1], argv[2]);
        break;
    default:
        perror("File is of an incorrect type.");
        return 1;
    }
    
	return 0;
}
