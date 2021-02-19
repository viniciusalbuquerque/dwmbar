#ifndef PROGRAM_H
#define PROGRAM_H
typedef struct program {
    const char* command;
    //char* attrs[];
    const char** attrs;
} PROGRAM;

char* program_execute(PROGRAM* program);
#endif
