#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define MAX 200
#define SLEEP_TIME 5
#define NUMBER_TRIES 5
#define NAMED_PIPE "public"

typedef struct{
    int cmd;
    char private_pipe[MAX];    
} request;
