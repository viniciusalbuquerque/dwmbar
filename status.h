typedef struct {
    const char* command;
} Program;

typedef struct {
    int order;
    const char* title;
    const char* command;
    int wait_time;
} Item ;

static const Item items[] = {
    {0, "Vol: ", "./barvol", 10},
    {1, "", "date \"+%a %k:%M %d/%m/%y\"", 30}
};

static const char* delimeter = " | ";

