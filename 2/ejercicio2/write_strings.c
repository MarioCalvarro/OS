#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
    FILE* file = NULL;
    char aux0 = '\0';
    char auxN = '\n';

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file_name> <string1> <string2> ... <stringN>\n", argv[0]);
        exit(1);
    }

    if ((file = fopen(argv[1], "w")) == NULL)
        err(2, "The output file %s could not be opened/created", argv[1]);

    for (int i = 2; i < argc; i++) {
        fwrite(argv[i], strlen(argv[i]), 1, file);
        fwrite(&aux0, sizeof(char), 1, file);
    }

    fwrite(&auxN, sizeof(char), 1, file);
    fclose(file);
    return 0;
}
