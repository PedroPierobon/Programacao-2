#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "vinac.h"

/* Cria diretaório e aloca N InfoMembers */ 
struct directory* cria_diretorio(int N);

/* Retorna um diretório lido dentro do archive */
struct directory* ler_diretorio(FILE* archive);

/* Encontra o membro de NOME e retorna a UID/POS */
/* ou -1 caso não encontrado */
int encontra_membro(struct directory* dir, const char* nome);

/* Substitui o membro encontrado pelo que está no archive */
/* Se do mesmo tamanho só substitui, se maior abre espaço atualizando offsets */
/* Se menor shift left nos posteriores para encurtar o arquivo */
void substitui_membro(FILE* archive, struct directory* dir, const char* name, int idx);

/* fwrite para escrever o Int N e o vetor de infoMember */
void escreve_diretorio(FILE* archive, struct directory* dir);

/* Escreve na última posição o membro de nome member_name */
/* Dir deve ter sido alocado espaço para a última posição */
void append_diretorio(struct directory* dir, const char* member_name);

// Passa pelo vetor de membros atualizando offset conforme a alocação do vetor
void atualiza_offset(struct directory* dir);

// Substitui o membro com mesmo nome pelo membro externo comprimido
void substitui_membro_comp(FILE* archive, struct directory* dir, const char* name, int idx);

// Append comprimido
void append_diretorio_comp(struct directory* dir, const char* filename);

#endif