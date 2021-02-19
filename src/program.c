#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "program.h"

#define MAX_ATTR_CHAR_SIZE 255

char* program_build_full_command(const char* command, const char** attrs) {
    if (!attrs) return (char*) command;
    char* full_command = malloc(sizeof(char) * MAX_ATTR_CHAR_SIZE);
    memset(full_command, '\0', MAX_ATTR_CHAR_SIZE);
    strcat(full_command, command);
    for (int i = 0; attrs[i]; i++) {
        strcat(full_command, " ");
        strcat(full_command, attrs[i]);
    }
    return full_command;
}

char* program_execute(PROGRAM* program) {
    char* full_command = program_build_full_command(program->command, program->attrs);
    printf("command: %s\n", full_command);
    
    char buffer[BUFSIZ + 1];
    memset(buffer, '\0', sizeof(buffer));
    FILE* fp = popen(full_command, "r");

    if (fp == NULL) {
        printf("Error popen");
        exit(EXIT_FAILURE);
    }

    int chars_read = fread(buffer, sizeof(char), BUFSIZ, fp);
    int pstatus = pclose(fp);
    if (chars_read > 0) {
        char* returnBuffer = malloc(sizeof(char) * (BUFSIZ + 1));
        memset(returnBuffer, '\0', BUFSIZ+1);
        strcpy(returnBuffer, buffer);
        return returnBuffer;
    }
    if (pstatus < 0) printf("Error closing file");
    return NULL;

}

/*int main() {
    const char* attrs[] = { NULL };
    PROGRAM program = { "date", attrs };
    char* result = program_execute(&program);
    printf("result: %s\n", result);
    free(result);
    return 0;
}*/
