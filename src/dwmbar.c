#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dwmbar.h"
#include "program.h"

#define MAX_CHARS_FOR_ITEM 50
#define DELIMETER "|"
#define QUOTE "\""

char** items_text = NULL;

int num_of_items() {
    return sizeof(bar_items)/sizeof(BAR_ITEM);
}

void free_items_text() {
    if (!items_text) return;

    for (int i = 0; i < num_of_items(); i++) {
        free(items_text[i]);
    }
    free(items_text);
}

void remove_break_line_from_end(char* str) {
    for (int i = strlen(str) - 1; str[i] == '\n'; i--) {
        str[i] = '\0';
    }
}

char** get_current_items_text() {
    if (items_text != NULL) return items_text;
    int n_items = num_of_items();
    items_text = malloc(sizeof(char*) * n_items);
    for (int i = 0; i < n_items; i++) {
        items_text[i] = malloc(sizeof(char) * MAX_CHARS_FOR_ITEM + 1);
        memset(items_text[i], '\0', MAX_CHARS_FOR_ITEM + 1);
    }
    return items_text;
}

char* build_item_string(char* result, const char* title, int order) {
    int size = strlen(result) + strlen(title) + 4;
    char* final_text = malloc(sizeof(char) * size);
    memset(final_text, '\0', size);
    strcat(final_text, title);
    strcat(final_text, " ");
    strcat(final_text, result);
    remove_break_line_from_end(final_text);
    if (order == num_of_items() - 1) {
        strcat(final_text, QUOTE);
    } else {
        strcat(final_text, " ");
        strcat(final_text, DELIMETER);
    }
    return final_text;
}

void update_dwm_bar(char* result, const char* title, int order) {
    if (order < 0) return;
    char** text = get_current_items_text();
    free(text[order]);
    char* final_item_text = build_item_string(result, title, order);
    text[order] = final_item_text;
    xset.attrs = (const char**) text;
    program_execute(&xset);
}

void* execute_program_in_thread(void* ptr_bar_item) {
    printf("execute_program_in_thread called\n");
    BAR_ITEM* item = (BAR_ITEM*) ptr_bar_item;
    printf("pr: %s\n", item->program->command);
    for(;;) {
        char* result = program_execute(item->program);
        update_dwm_bar(result, item->title, item->order);
        free(result);
        sleep(item->delay);
    }
    free(item);
    pthread_exit(NULL);
}

void create_threads_for_items() {
    int num_items = num_of_items();
    pthread_t pthread_ids[num_items];
    BAR_ITEM* aux_bar_items[num_items];
    for (int i = 0; i < num_items; i++) {
        BAR_ITEM* bar_item = malloc(sizeof(BAR_ITEM));
        bar_item->title = bar_items[i].title;
        bar_item->order = bar_items[i].order;
        bar_item->delay = bar_items[i].delay;
        bar_item->program = bar_items[i].program;
        pthread_create(&pthread_ids[i], NULL, execute_program_in_thread, bar_item);
        aux_bar_items[i] = bar_item;
    }

    for (int i = 0; i < num_items; i++) {
        pthread_join(pthread_ids[i], NULL);
    }

    for (int i = 0; i < num_items; i++) {
        free(aux_bar_items[i]);
    }
}

int main() {
    create_threads_for_items();
    free_items_text();
    return 0;
}
