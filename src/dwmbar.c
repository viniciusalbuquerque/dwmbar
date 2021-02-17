#include <stdio.h>
#include <stdlib.h>
#include "./dwmbar.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LENGTH(X)               (sizeof(X) / sizeof (X[0]))
#define NUM_OF_ITEMS            (LENGTH(items))
#define DELIMETER_LEN           (strlen(delimeter))
#define X_SET_ROOT              "xsetroot"
#define X_SET_ROOT_NAME         "-name"
#define QUOTES                  "\""
#define QUOTES_SIZE             1 

char** status_data;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void setStatusBarText() {
    int textSize = getAttrsTextSize();
    textSize += 2*QUOTES_SIZE; 
    char* command = buildAttrTextForXRoot(textSize);

    printf("final: %s\n", command);

    pthread_mutex_lock(&mutex);
    Program p = { command };
    char* result = executeProgram(p);
    pthread_mutex_unlock(&mutex);

    free(command);
    free(result);
}

char* buildAttrTextForXRoot(int attrsSize) {
    int textSize = strlen(X_SET_ROOT) + strlen(X_SET_ROOT_NAME) + 2; // +2 for spaces between xsetroot and -name, -name and values
    textSize += attrsSize;

    char* attrText = malloc(sizeof(char*) * textSize);
    memset(attrText, '\0', textSize);
    strcat(attrText, X_SET_ROOT);
    strcat(attrText, " ");
    strcat(attrText, X_SET_ROOT_NAME);
    strcat(attrText, " ");
    strcat(attrText, QUOTES);
    for (int i = 0; i < NUM_OF_ITEMS; i++) {
        Item item = items[i];
        const char* title = item.title;
        strcat(attrText, title);
        if (status_data[i] != NULL && strlen(status_data[i]) > 0) {
            strcat(attrText, status_data[i]);
        }
        if (i != NUM_OF_ITEMS - 1) 
            strcat(attrText, delimeter);
    }
    strcat(attrText, QUOTES);
    return attrText;
}

char* getNoNewLineText(char* text) {
    if (text == NULL) return text;
    int textLength = strlen(text);
    if (textLength == 0) return text;
    if (text[textLength - 1] == '\n')
        text[textLength - 1] = '\0';
    return text;
}

int getAttrsTextSize() {
    int textSize = strlen(X_SET_ROOT_NAME);
    if (status_data == NULL) return textSize;
    for (int i = 0; i < NUM_OF_ITEMS; i++) {
        Item item = items[i];
        textSize += strlen(item.title);
        if (status_data[i] == NULL) continue;
        getNoNewLineText(status_data[i]);
        long int dataSize = strlen(status_data[i]);
        textSize += dataSize;
        if (i != NUM_OF_ITEMS - 1)
            textSize += DELIMETER_LEN;
    }
    return textSize;
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
        memset(returnBuffer, '\0', BUFSIZ+1);
        strcpy(returnBuffer, buffer);
        return returnBuffer;
    }
    if (pstatus < 0) printf("Error closing file");
    return NULL;
}

char* executeProgramForItem(Item item) {
    Program program;
    program.command = item.command;
    char* result = executeProgram(program);
    return result;
}

void* statusUpdate(void* arg) {
    Item* item = (Item *) arg;
    int wait_time = item->wait_time;
    for(;;) {
        char* result = executeProgramForItem(*item);
        updateStatusBar(result, item->order);
        free(result);
        sleep(wait_time);
    }
    pthread_exit(NULL);
}

int main(int argc, char** args) {

    status_data = (char**) malloc(sizeof(char*) * NUM_OF_ITEMS);
    for(int i = 0; i < NUM_OF_ITEMS; i++)
        status_data[i] = malloc(sizeof(char*)*BUFSIZ);

    pthread_t *pthread_group = malloc(sizeof(pthread_t) * NUM_OF_ITEMS);

    Item* item;
    for (int i = 0; i < NUM_OF_ITEMS; i++) {
        item = malloc(sizeof(Item));
        item->title = items[i].title;
        item->command = items[i].command;
        item->order = items[i].order;
        item->wait_time = items[i].wait_time;
        pthread_create(&pthread_group[i], NULL, statusUpdate, item);
    }

    for (int i = 0; i < NUM_OF_ITEMS; i++) {
        pthread_join(pthread_group[i], NULL);
    }

    free(item);

    free(pthread_group);
    for(int i = 0; i < NUM_OF_ITEMS; i++)
        free(status_data[i]);
    free(status_data);
    return 0;
}
