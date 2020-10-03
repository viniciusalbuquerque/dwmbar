#include <stdio.h>
#include <stdlib.h>
#include "./status.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define LENGTH(X)               (sizeof(X) / sizeof (X[0]))
#define NUM_OF_ITEMS            (LENGTH(items))
#define DELIMETER_LEN           (strlen(delimeter))
#define X_SET_ROOT              "xsetroot -name "
#define QUOTES                  "\""
#define QUOTES_SIZE             1 

char** status_data;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char* executeProgram(Program program);

void setStatusBarText() {
    int textSize = strlen(X_SET_ROOT);
    
    for (int i = 0; i < NUM_OF_ITEMS; i++) {
        Item item = items[i];
        const char* title = item.title;
        textSize += strlen(title);
        char* data = status_data[i];
        long int dataSize = strlen(data);
        if (data != NULL && dataSize != 0) {
            if (data[dataSize - 1] == '\n') {
                data = strtok(data, "\n");
                dataSize = strlen(data);
            }
            textSize += dataSize;
        }
        if (i != NUM_OF_ITEMS - 1)
            textSize += DELIMETER_LEN;
    }
    textSize += 2*QUOTES_SIZE;

    char buffer[textSize];
    strcat(buffer, X_SET_ROOT);
    strcat(buffer, QUOTES);
    for (int i = 0; i < NUM_OF_ITEMS; i++) {
        Item item = items[i];
        const char* title = item.title;
        strcat(buffer, title);
        char* data = status_data[i];
        long int dataSize = strlen(data);
        if (data != NULL && dataSize > 0) {
            if (data[dataSize - 1] == '\n') {
                data = strtok(data, "\n");
                dataSize = strlen(data);
            }
            strcat(buffer, data);
        }

        if (i != NUM_OF_ITEMS - 1) 
            strcat(buffer, delimeter);
    }
    strcat(buffer, QUOTES);

    printf("final: %s\n", buffer);

    const char* text = buffer;
    pthread_mutex_lock(&mutex);
    Program p = { text };
    char* result = executeProgram(p);
    free(result);
    pthread_mutex_unlock(&mutex);

}

void updateStatusBar(char *buffer, int order) {
    pthread_mutex_lock(&mutex);
    strncpy(status_data[order], buffer, strlen(buffer));
    pthread_mutex_unlock(&mutex);
    setStatusBarText();
}

char* executeProgram(Program program) {
    char buffer[BUFSIZ + 1];
    memset(buffer, '\0', sizeof(buffer));
    FILE* fp = popen(program.command, "r");

    if (fp == NULL) {
        printf("Error popen");
        exit(EXIT_FAILURE);
    }

    int chars_read = fread(buffer, sizeof(char), BUFSIZ, fp);
    int pstatus = pclose(fp);
    if (chars_read > 0) {
        char* returnBuffer = malloc(sizeof(char) * (BUFSIZ + 1));
        return returnBuffer;
    }
    if (pstatus < 0) printf("Error closing file");
    return NULL;
}

void executeProgramForItem(Item item) {
    Program program;
    program.command = item.command;
    char* result = executeProgram(program);
    updateStatusBar(result, item.order);
    free(result);
}

void* statusUpdate(void* arg) {
    Item item = *(Item *) arg;
    int wait_time = item.wait_time;
    for(;;) {
        executeProgramForItem(item);
        sleep(wait_time);
    }
    pthread_exit(NULL);
}

int main(int argc, char** args) {

    status_data = (char**) malloc(sizeof(char*) * NUM_OF_ITEMS);
    for(int i = 0; i < NUM_OF_ITEMS; i++)
        status_data[i] = (char*)malloc(sizeof(char*));

    pthread_t *pthread_group = malloc(sizeof(pthread_t) * NUM_OF_ITEMS);

    Item* item;
    for (int i = 0; i < NUM_OF_ITEMS; i++) {
        item = malloc(sizeof(Item));
        (*item).title = items[i].title;
        (*item).command = items[i].command;
        (*item).order = items[i].order;
        (*item).wait_time = items[i].wait_time;
        pthread_create(&pthread_group[i], NULL, statusUpdate, item);
    }

    for (int i = 0; i < NUM_OF_ITEMS; i++) {
        pthread_join(pthread_group[i], NULL);
    }

    free(item);

    for(int i = 0; i < NUM_OF_ITEMS; i++)
        free(status_data[i]);
    free(status_data);
    return 0;
}
