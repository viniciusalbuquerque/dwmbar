#ifndef DWMBAR_H
#define DWMBAR_H

#include "program.h"

typedef struct bar_item {
    const char* title;
    int order;
    int delay;
    PROGRAM* program;
} BAR_ITEM;

static PROGRAM xset = {
    "xsetroot -name \"",
    0
};

static PROGRAM vol = {
    "barvol",
    0
};

static const char* date_attrs[] = { "\'+%a %H:%M %d/%m/%y\'", 0 };
static PROGRAM date = {
    "date",
    date_attrs
};

static const BAR_ITEM bar_items[] = {
    {"Vol: ", 0, 1, &vol },
    {"", 1, 2, &date }, 
};

#endif
