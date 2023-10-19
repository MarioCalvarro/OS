#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file = NULL;
    char* txt = (char *)malloc(sizeof(char));

	if (argc != 2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2, "The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */
	while (fread(txt, sizeof(char), 1, file) == 1) {
		/* Print byte to stdout */
        //Si no se ha escrito nada es porque ha ocurrido un fallo
		if (fwrite(txt, sizeof(char), 1, stdout) != 1){
			fclose(file);
			err(3, "putc() failed!!");
		}
	}

	fclose(file);
	return 0;
}
