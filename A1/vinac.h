#ifndef VINAC_H
#define VINAC_H

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_NAME_SIZE 1024
#define MAX_BUFFER_SIZE 1024

struct infoMember {
    char name[MAX_NAME_SIZE];
    off_t originalSize;
    long diskSize;
    time_t modTime;
    int pos;
    long offset;
};

struct directory {
    int N;
    struct infoMember *members;
};

#endif
