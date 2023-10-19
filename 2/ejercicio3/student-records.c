#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include <err.h>

#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

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
char * loadstr(FILE* file)
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

void free_entry(student_t *entry) {
    free(entry->first_name);
    free(entry->last_name);
    free(entry);
}

student_t * parse_line(FILE *file) {
    student_t* cur_entry = malloc(sizeof(student_t));
    memset(cur_entry, 0, sizeof(student_t));

    char* token;
    char line[MAXLEN_LINE_FILE+1];
    char* lineptr;

    token_id_t token_id = STUDENT_ID_IDX;

    fgets(line, MAXLEN_LINE_FILE + 1, file);
    lineptr = line;
    while((token = strsep(&lineptr, ":"))) {
        switch (token_id) {
        case STUDENT_ID_IDX:
            if (sscanf(token, "%d", &cur_entry->student_id) != 1) {
                fprintf(stderr, "Warning: Wrong format for student_id field\n");
                cur_entry->student_id = 0;
            }
            break;
        case NIF_IDX:
            strcpy(cur_entry->NIF, token);
            break;
        case FIRST_NAME_IDX:
            cur_entry->first_name = strdup(token);
            break;
        case LAST_NAME_IDX:
            //To remove the newline
            token[strlen(token) - 1] = '\0';
            cur_entry->last_name = strdup(token);
            break;
        default:
            break;
        }
        token_id++;
    }

    return cur_entry;
}

int print_text_file(char *path)
{
    FILE* file = NULL;
    if ((file = fopen(path, "r")) == NULL) {
        err(2, "The input file %s could not be opened", path);
    }

    size_t num_rows = 0;
    if (fscanf(file, "%zu", &num_rows) != 1) {
        fprintf(stderr, "Error: Could not read the number of entries.");
        return 1;
    } 

    //Skip newline character
    getc(file);
    for (size_t i = 0; i < num_rows; i++) {
        printf("[Entry #%zu]\n", i);
        student_t* cur_entry = parse_line(file); 
        printf("\tstudent_id=%d\n\tNIF=%s\n\tfirst_name=%s\n\tlast_name=%s\n",
                cur_entry->student_id, cur_entry->NIF, cur_entry->first_name, cur_entry->last_name);
        free_entry(cur_entry);
    }
    
    fclose(file);
	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
    FILE* input_stream = NULL;
    FILE* output_stream = NULL;

    if ((input_stream = fopen(input_file, "r")) == NULL) {
        err(2, "The input file %s could not be opened", input_file);
    }

    if ((output_stream = fopen(output_file, "w")) == NULL) {
        err(2, "The output file %s could not be opened", output_file);
    }

    int num_rows = 0;
    if (fscanf(input_stream, "%d", &num_rows) != 1) {
        fprintf(stderr, "Error: Could not read the number of entries.");
        return 1;
    }

    fwrite(&num_rows, sizeof(int), 1, output_stream);

    //Skip newline character
    getc(input_stream);
    for (size_t i = 0; i < num_rows; i++) {
        student_t* cur_entry = parse_line(input_stream);

        fwrite(&(cur_entry->student_id), sizeof(int), 1, output_stream);

        fwrite(&(cur_entry->NIF), strlen(cur_entry->NIF), 1, output_stream);
        fwrite(&"\0", sizeof(char), 1, output_stream);
        
        fwrite(cur_entry->first_name, strlen(cur_entry->first_name), 1, output_stream);
        fwrite(&"\0", sizeof(char), 1, output_stream);

        fwrite(cur_entry->last_name, strlen(cur_entry->last_name), 1, output_stream);
        fwrite(&"\0", sizeof(char), 1, output_stream);
    }

    fclose(input_stream);
    fclose(output_stream);
	return 0;
}


int print_binary_file(char *path)
{
    FILE* file = NULL;
    student_t* cur_entry = malloc(sizeof(student_t));
    memset(cur_entry, 0, sizeof(student_t));

    if ((file = fopen(path, "r")) == NULL) {
        err(2, "The input file %s could not be opened", path);
    }


    int num_rows = 0; 
    fread(&num_rows, sizeof(int), 1, file);

    char* aux;
    for (size_t i = 0; i < num_rows; i++) {
        printf("[Entry #%zu]\n", i);

        //student_id is an int
        fread(&(cur_entry->student_id), sizeof(int), 1, file);

        //Because nif is of predefined size
        aux = loadstr(file);
        for (size_t ii = 0; ii < sizeof(cur_entry->NIF); ii++) {
            cur_entry->NIF[ii] = aux[ii];
        }

        cur_entry->first_name = loadstr(file);
        cur_entry->last_name = loadstr(file);

        printf("\tstudent_id=%d\n\tNIF=%s\n\tfirst_name=%s\n\tlast_name=%s\n",
                cur_entry->student_id, cur_entry->NIF, cur_entry->first_name, cur_entry->last_name);
    }

    free_entry(cur_entry);
    fclose(file);
	return 0;
}


int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hbpo:i:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i <input_file> ] | -o <output_file> | -b ] \n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
            options.input_file = optarg;
			break;
        case 'p':
            options.action = PRINT_TEXT_ACT;
            break;
        case 'o':
            options.action = WRITE_BINARY_ACT;
            options.output_file = optarg;
            break;
        case 'b':
            options.action = PRINT_BINARY_ACT;
            break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
