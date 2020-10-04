typedef struct {
    const char* command;
//    const char* attrs;
} Program;

typedef struct {
    int order;
    const char* title;
    const char* command;
    int wait_time;
} Item ;

static const Item items[] = {
/*  order, title, script, time_interval */
/*
 *  Note the even though we could change the order in the first element,
 *  this program is using this value more as an id to be able to do some
 *  information handling. So, if you want to change the order, change the
 *  actual order here in the array and also change the 'order' attribute to
 *  its correspondent place.
 *
 */
    {0, "Vol: ", "./barvol", 10},
    {1, "", "date \'+%a %k:%M %d/%m/%y\'", 30}
};

/*static const Program xsetroot = {
    "xsetroot", {"-name", ""}
};*/

static const char* delimeter = " | ";
