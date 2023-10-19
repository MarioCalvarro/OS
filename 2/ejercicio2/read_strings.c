#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE* file)
{
    char* ret = (char *) malloc(sizeof(char));
    int len = 0;

    //Check length of string
    if (fread(ret, sizeof(char), 1, file) != 1 || *ret == '\n') {
        return NULL;
    }

    while (*ret != '\0') {
        len++;
        fread(ret, sizeof(char), 1, file);
    }
    free(ret);

    //Allocate memory of string
    ret = (char *)malloc(sizeof(char) * len);

    //Go back to beginning of string
    fseek(file, -len - 1, SEEK_CUR);
    
    //Copy string to the pointer
    fread(ret, len + 1, 1, file);

    return ret;
}

int main(int argc, char *argv[])
{
    FILE* file = NULL;
    char* txt = NULL;
    char auxN = '\n';
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(1);   
    } 

    if ((file = fopen(argv[1], "r")) == NULL)
        err(2, "The input file %s could not be opened", argv[1]);

    while((txt = loadstr(file)) != NULL) {
        fwrite(txt, strlen(txt), 1, stdout);
        fwrite(&auxN, sizeof(char), 1, stdout);
        free(txt);
    }

    fclose(file);
	return 0;
}
