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
char* loadstr(FILE* file)
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

/** Frees the memory pointed by entries 
 * (which is sizeof(student_t) * nr_entries) and the memory allocated for the
 * strings inside
 */
void free_entries(student_t* entries, int nr_entries) {
    for (size_t i = 0; i < nr_entries; i++) {
        free(entries[i].first_name);
        free(entries[i].last_name);
    }
    free(entries);
}

/** Reads a line of the input file to cur_entry
 *
 * file: Pointer to the FILE descriptor
 *
 * This function assumes that cur_entry has been allocated exactly
 * sizeof(student_t) bytes in memory.
 */
void parse_line(FILE* file, student_t* cur_entry) 
{
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
}

/** Reads the file with the entries of the students, including the number of 
 * students
 *
 * file: Pointer to the FILE descriptor
 *
 * This function must allocated the necessary memory for students, but does not
 * free it.
 */
student_t* read_student_text_file(FILE* students, int* nr_entries)
{
    if (fscanf(students, "%d", nr_entries) != 1) {
        fprintf(stderr, "Error: Could not read the number of entries.");
        exit(1);
    } 

    //Skip newline character
    getc(students);

    student_t* res = malloc(sizeof(student_t) * *nr_entries);
    memset(res, 0, sizeof(student_t) * *nr_entries);

    for(size_t i = 0; i < *nr_entries; i++) {
        parse_line(students, &res[i]);
    }

    return res; 
}

/** Reads a line of the input binary file to cur_entry
 *
 * file: Pointer to the FILE descriptor
 *
 * This function assumes that cur_entry has been allocated exactly
 * sizeof(student_t) bytes in memory.
 */
void parse_binary_line(FILE* file, student_t* cur_entry)
{
    //student_id is an int
    fread(&(cur_entry->student_id), sizeof(int), 1, file);

    //Because nif is of predefined size
    char* aux = loadstr(file);
    for (size_t ii = 0; ii < sizeof(cur_entry->NIF); ii++) {
        cur_entry->NIF[ii] = aux[ii];
    }
    free(aux);

    cur_entry->first_name = loadstr(file);
    cur_entry->last_name = loadstr(file);
}

/** Reads the binary file with the entries of the students, including 
 * the number of students
 *
 * file: Pointer to the FILE descriptor
 *
 * This function must allocated the necessary memory for students, but does not
 * free it.
 */
student_t* read_student_binary_file(FILE* students, int* nr_entries) 
{
    fread(nr_entries, sizeof(int), 1, students);
    student_t* res = malloc(sizeof(student_t) * *nr_entries);
    memset(res, 0, sizeof(student_t) * *nr_entries);

    for(size_t i = 0; i < *nr_entries; i++) {
        parse_binary_line(students, &res[i]);
    }

    return res;
}

/** Prints the entries from the file in the path
 */
int print_text_file(char *path)
{
    FILE* file = NULL;
    if ((file = fopen(path, "r")) == NULL) {
        err(2, "The input file %s could not be opened", path);
    }

    int num_rows = 0;
    student_t* entries = read_student_text_file(file, &(num_rows));
    for (size_t i = 0; i < num_rows; i++) {
        printf("[Entry #%zu]\n", i);
        printf("\tstudent_id=%d\n\tNIF=%s\n\tfirst_name=%s\n\tlast_name=%s\n",
                entries[i].student_id, entries[i].NIF, entries[i].first_name, entries[i].last_name);
    }
    
    free_entries(entries, num_rows);
    fclose(file);
	return 0;
}

/** Writes the entries in the input_file to a binary file in output_file
 */
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
    student_t* entries = read_student_text_file(input_stream, &num_rows);

    fwrite(&num_rows, sizeof(int), 1, output_stream);

    for (size_t i = 0; i < num_rows; i++) {
        student_t* cur_entry = &entries[i];

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

/** Prints the entries in a binary file in path with the structure of the file
 * written in write_binary_file()
 */
int print_binary_file(char* path)
{
    FILE* file = NULL;

    if ((file = fopen(path, "r")) == NULL) {
        err(2, "The input file %s could not be opened", path);
    }

    int num_rows = 0;
    student_t* entries = read_student_binary_file(file, &num_rows);
    for (size_t i = 0; i < num_rows; i++) {
        student_t* cur_entry = &entries[i];

        printf("[Entry #%zu]\n", i);
        printf("\tstudent_id=%d\n\tNIF=%s\n\tfirst_name=%s\n\tlast_name=%s\n",
                cur_entry->student_id, cur_entry->NIF, cur_entry->first_name, cur_entry->last_name);
    }

    free_entries(entries, num_rows);
    fclose(file);
	return 0;
}

int append_to_text_file(FILE* file, char* argv[], size_t index)
{
    while (argv[index] != NULL) {
        fprintf(file, "%s\n", argv[index]);
        index++;
    }
    return 0;
}

int append_to_binary_file(FILE* file, char* argv[], size_t index)
{
    char* token;
    int aux;
    int count = 0;
    while (argv[index] != NULL) {
        char* lineptr = argv[index];
        token_id_t token_id = STUDENT_ID_IDX;
        //TODO: Change char* to int, 
        while((token = strsep(&lineptr, ":"))) {
            switch (token_id) {
            case STUDENT_ID_IDX:
                aux = atoi(token);
                fwrite(&aux, sizeof(int), 1, file);
                break;
            case NIF_IDX:
                fwrite(token, strlen(token), 1, file);
                fwrite(&"\0", sizeof(char), 1, file);
                break;
            case FIRST_NAME_IDX:
                fwrite(token, strlen(token), 1, file);
                fwrite(&"\0", sizeof(char), 1, file);
                break;
            case LAST_NAME_IDX:
                fwrite(token, strlen(token), 1, file);
                fwrite(&"\0", sizeof(char), 1, file);
                break;
            default:
                break;
            }
            token_id++;
        }
        index++;
        count++;
    }
    return 0;
}

int append_to_file(char* path, char* argv[], size_t index)
{
    FILE* file = NULL;

    if ((file = fopen(path, "a")) == NULL) {
        err(2, "The output file %s could not be opened", path);
    }

    //We assume the extension is .txt or .bin and nothing else
    char* extension = &path[strlen(path) - 3];
    switch (*extension)
    {
        case 't':
            return append_to_text_file(file, argv, index);
        case 'b':
            return append_to_binary_file(file, argv, index);
        default:
            return 1;
    }   
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
	while ((opt = getopt(argc, argv, "hbpo:i:a")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i <input_file> ] | -o <output_file> | -b | -a ] \n", argv[0]);
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
        case 'a':
            options.action = APPEND_TEXT_ACT;
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
    case APPEND_TEXT_ACT:
        /* Part Opt2 */
        ret_code = append_to_file(options.input_file, argv, optind);
        break;
	default:
		break;
	}
	exit(ret_code);
}
