#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks)
{
    struct stat st;
    lstat(fname, &st);
    if (S_ISDIR(st.st_mode)) {
        get_size_dir(fname, blocks);
    }
    *blocks += st.st_blocks;
    return 0;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{
    DIR* dir;
    struct dirent* entry;
    dir = opendir(dname);
    char fname[512];
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue; 
        }
        sprintf(fname, "%s/%s", dname, entry->d_name);
        size_t aux = 0;
        get_size(fname, &aux);
        *blocks += aux;
    }
    closedir(dir);
    return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
    if (argc <= 1) {
        perror("Incorrect number of arguments (1 or more are requiered).");
        exit(EXIT_FAILURE);
    }

    size_t blocks;
    for (int i = 1; i < argc; i++) {
        get_size(argv[i], &blocks);
        printf("%luK\t%s\n", blocks/2, argv[i]);
    }

	return 0;
}
