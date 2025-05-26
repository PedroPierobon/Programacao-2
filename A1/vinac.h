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
    size_t N;
    struct infoMember *members;
};

// Insere membro no archive.vc
void insere(FILE* archive, int N_novos, char* novos[], struct directory* dir);

// Insere comprimido
void insere_comp(FILE* archive, int N_novos, char* novos[], struct directory* dir);

// Extrai
void extract(FILE* archive, struct directory* dir, int argc, char* argv[]);

// Move Membro para frente do Target
void move_member(FILE* archive, const char* member, const char* target, struct directory* dir, const char* archive_name);

// Remove
void remove_membro(FILE* archive, struct directory* dir, size_t idx);

// Printa todos os Membros do arquivo
void printar(struct directory* dir);

#endif
