#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "vinac.h"
#include "directory.h"

/* Tenta abrir o arquivo com nome archive_name, se existir, lê e atualiza dir */
/* Caso não exista vai criar um dir com 0 membros.*/
FILE* abre_arquivo (const char* archive_name, struct directory** dir);

/* Copia o membro iniciando em source de tamanho size para o offset dest */
void shift_right_archive(FILE* archive, long source, long dest, size_t size);

void shift_left_archive(FILE* archive, long source, long dest,  size_t size);

// Escreve Membro no ponteiro onde archive está
void escreve_membro(FILE* archive, const char *member_name);

// Extrai membro
void extrai_membro(FILE* archive, struct infoMember* membro);

void escreve_membro_comp(FILE* archive, const char* filename);

#endif