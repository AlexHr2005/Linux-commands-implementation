#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char* VALID_FLAGS[] = {
    "-c"
};

const int VALID_FLAGS_N = 1;

enum boolean {
    FALSE,
    TRUE
};

#define BUFFER_SIZE 20

//need to check if the file given is directory or not
int extract_files(int argc, char* argv[], char*** files, int* curr_arg) {
    int filePaths_C = 0;
    for(int i = *curr_arg; i < argc; i++) {
        char* arg = argv[i];
        char* currFilePath;
        if(arg[0] == '/') {
            currFilePath = (char*) malloc((strlen(arg) + 1) * sizeof(char));
            strcpy(currFilePath, arg);
        }
        else{
            char* cwd = getcwd(NULL, 0);
            currFilePath = realloc(cwd, (strlen(cwd) + 1 + strlen(arg) + 1) * sizeof(char));
            strcat(currFilePath, "/");
            strcat(currFilePath, arg);
        }
        if(access(currFilePath, F_OK) == -1) {
            if(errno == ENOENT) {
                printf("%s: The file does not exist.\n", currFilePath);
                free(currFilePath);
                exit(1);
            }
        }
        if(*files == NULL) {
            *files = (char**) malloc(sizeof(char*));
        }
        else {
            *files = realloc(*files, (filePaths_C + 1) * sizeof(char*));
        }
            
        (*files)[filePaths_C] = (char*) malloc((strlen(currFilePath) + 1) * sizeof(char));
        strcpy((*files)[filePaths_C], currFilePath);
        filePaths_C++;
        free(currFilePath);
    }
    return filePaths_C;
}

//raboti dokato ne nameri 1 pravilen flag
int extract_flags(int argc, char* argv[], char*** flags, int* curr_arg_index) {
    int flags_C = 0;
    for(int i = *curr_arg_index; i < argc; i++) {
        (*curr_arg_index)++;
        if(flags_C > 0) {
            break;
        }
        char* arg = argv[i];

        if(!strcmp(arg, "--")) {
            return flags_C;
        }
        if(arg[0] == '-') {
            enum boolean is_flag_valid = FALSE;
            for(int j = 0; j < VALID_FLAGS_N; j++) {
                if(!strcmp(arg, VALID_FLAGS[j])) {
                    if(*flags == NULL) {
                        *flags = (char**) malloc((sizeof(char*)));
                    }
                    else {
                        *flags = realloc(*flags, (flags_C + 1) * sizeof(char*));  
                    }

                    (*flags)[flags_C] = (char*) malloc((strlen(arg) + 1) * sizeof(char));
                    strcpy((*flags)[flags_C], arg);
                    flags_C++;
                    is_flag_valid = TRUE;
                    break;
                }
            }
            if(!is_flag_valid) {
                printf("Invalid option: '%s'\n", arg);
            }
        }
        else {
            (*curr_arg_index)--;
            return flags_C;
        }
    }
    return flags_C;
}

void extract_pattern(char* argv[], char** pattern, int* curr_arg) {
    *pattern = (char*) malloc((strlen(argv[*curr_arg]) + 1) * sizeof(char));
    strcpy(*pattern, argv[*curr_arg]);
    (*curr_arg)++;
}

int findPatternInLine(char* line, char* pattern) {
    int indexLine = 0;
    int indexPattern = 0;
    while(line[indexLine] != '\0') {
        if(pattern[indexPattern] == '\0') {
            return TRUE;
        }
        if(line[indexLine] == pattern[indexPattern]) {
            indexPattern++;
        }
        else indexPattern = 0;
        indexLine++;
    }
    if(pattern[indexPattern] == '\0') {
        return TRUE;
    }
    return FALSE;
}

void printMatchedLines(char** files, int files_C, char* pattern) {
    for(int i = 0; i < files_C; i++) {
        int fd = open(files[i], O_RDONLY);

        if(fd == -1) {
            printf("Error occurred with opening file %s\n", files[i]);
            close(fd);
            continue;
        }

        char buffer[BUFFER_SIZE];
        int bytesRead;

        enum boolean new_line_encounter = FALSE;
        enum boolean pattern_found = FALSE;

        char* lineToBePrinted = (char*) malloc (50 * sizeof(char));
        int sizeOfLine = 50;
        int indexLine = 0;
        while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
            if(bytesRead < 0) {
                printf("Error occured with reading from file %s\n", files[i]);
                close(fd);
                break;
            }
            for(int i = 0; i < bytesRead; i++) {
                if(buffer[i] == '\r' || buffer[i] == '\n') {
                    new_line_encounter = TRUE;
                    lineToBePrinted[indexLine] = '\0';
                    indexLine = 0;
                    if(findPatternInLine(lineToBePrinted, pattern) == TRUE) {
                        printf("%s\n", lineToBePrinted);
                    }
                    if(i != BUFFER_SIZE - 1 && buffer[i] == '\r' && buffer[i + 1] == '\n') {
                        i++;
                    }
                    continue;
                }
                if(indexLine == sizeOfLine - 1) {
                    lineToBePrinted = realloc(lineToBePrinted, sizeOfLine * 2 * sizeof(char));
                    sizeOfLine *= 2;
                }
                lineToBePrinted[indexLine] = buffer[i];
                indexLine++;
            }
        }
        lineToBePrinted[indexLine] = '\0';
        if(findPatternInLine(lineToBePrinted, pattern) == TRUE) {
           printf("%s\n", lineToBePrinted);
        }

        close(fd);
    }
}

void execute(char** files, char** flags, int files_C, int flags_C, char* pattern) {
    if(flags_C == 0) {
        printMatchedLines(files, files_C, pattern);
        return;
    }
    
    

    char* flag = flags[0];

}

int main(int argc, char* argv[]) {
    char** files = NULL;
    char** flags = NULL;
    char* pattern = NULL;
    int curr_arg = 1;

    int flags_C = extract_flags(argc, argv, &flags, &curr_arg);
    extract_pattern(argv, &pattern, &curr_arg);
    int files_C = extract_files(argc, argv, &files, &curr_arg);

    execute(files, flags, files_C, flags_C, pattern);

    free(files);
    free(flags);
    free(pattern);

    return 0;
}