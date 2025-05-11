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

// Escreve Membro no ponteiro onde archive está
void escreve_membro(FILE* archive, const char *member_name);

// Cria diretaório e aloca N InfoMembers
struct directory* cria_diretorio(int N);

// Copia o membro iniciando em source de tamanho size para o offset dest
void shift_right_archive(FILE* archive, long source, long dest, size_t size);

// Copia o membro iniciando em source de tamanho size para o offset dest
void shift_left_archive(FILE* archive, long source, long dest,  size_t size);

// fwrite para escrever o Int N e o vetor de infoMember
void escreve_diretorio(FILE* archive, struct directory* dir);

// Retorna um diretório lido dentro do archive
struct directory* ler_diretorio(FILE* archive);

// Passa pelo vetor de membros atualizando offset conforme a alocação do vetor
void atualiza_offset(struct directory* dir);

// Escreve na última posição o membro de nome member_name
// Dir deve ter sido alocado espaço para a última posição
void append_diretorio(struct directory* dir, const char* member_name);

#endif
