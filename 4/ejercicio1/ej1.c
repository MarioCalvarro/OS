#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

struct options {
	char *progname;
	int recurse;
};

struct options opt;

void usage(void)
{
	printf("%s [options] [dirname]\n\n", opt.progname);
	printf("lists the contents of dirname (default .)\n");
	printf("options:\n"
		"\t-h:\tshow this help\n"
		"\t-R:\trecursive\n"
	);
}

/* apartado b */
void list_dir(char *name)
{
    DIR* d;
    struct dirent* de;
    d = opendir(name);

    while ((de = readdir(d)) != NULL) {
        printf("%s\n", de->d_name);
    }

    closedir(d);
}

/* apartado c */
void process_recurse(char *dirname, char *name)
{
	pid_t pid;
	char path[PATH_MAX];

    pid = fork();
    if (pid == 0) {
        snprintf(path, PATH_MAX, "%s/%s", dirname, name);
        execl(opt.progname, opt.progname, "-R", path, NULL);
        err(EXIT_FAILURE, "ERROR!");
    } else {
        wait(NULL);
    }
}

/* apartado c */
void list_dir_recurse(char *name)
{
	DIR *d;
	struct dirent *de;
    d = opendir(name);

    while ((de = readdir(d)) != NULL) {
        //Check whether it is a dir
        if (de->d_type == DT_DIR) {
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                continue;
            process_recurse(name, de->d_name);
        }
    }

    closedir(d);
}

int main(int argc, char **argv)
{
	char *dirname = ".";
	int o;
	opt.progname = argv[0];
	opt.recurse = 0;

	/* Apartado a: procesar opciones con getopt */
    while ((o = getopt(argc, argv, "hR")) != -1) {
        switch (o) {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
            break;
        case 'R':
            opt.recurse = 1;
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
        } 
    }
	/********************************************/
    if (optind < argc) {
        dirname = argv[optind];
    }

	if (opt.recurse)
		printf("%s:\n", dirname);

	list_dir(dirname);

	if (opt.recurse)
		list_dir_recurse(dirname);

	return 0;
}
