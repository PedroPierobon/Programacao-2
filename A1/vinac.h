#ifndef VINAC
#define VINAC

#include <stddef.h>
#include <time.h>

struct infoMember {
    char name[1024];
    int uid;
    size_t osize;
    size_t dsize;
    time_t modData;
    int pos;
    long offset;
}

struct directory {
    int N;
    struct infoMember *members;
}

void memberInsert(const char *member_name, const char *archive_name);
