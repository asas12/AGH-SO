#define MAX_NAME_SIZE 256

#define MAX_NO_OF_CLIENTS 10

enum myoperator {
    ADD, SUB, MUL, DIV
};

struct operation {
    enum myoperator op;
    int arg1;
    int arg2;
};


#define START 1
#define MIRROR 2
#define ADD 3
#define MUL 4
#define SUB 5
#define DIV 6
#define TIME 7
#define END 8
#define STOP 9

int SERVER_STOP;
int CLIENT_STOP;


struct myMsgBuf {
    long mType;
    pid_t pid;
    char msg[MAX_MSG_SIZE];
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};