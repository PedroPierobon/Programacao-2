#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "directory.h"
#include "archive.h"
#include "vinac.h"

// Insere membro no archive.vc
void insere(FILE* archive, int N_novos, char* novos[], struct directory* dir) {
    if (N_novos == 0) return;

    int old_N = dir->N;
    int new_N = old_N + N_novos;

    // Realloc para caber os novos membros
    struct infoMember* temp = realloc(dir->members, new_N * sizeof(struct infoMember));
    if (!temp) {
        perror("Erro ao realocar vetor de infoMember");
        return;
    }
    dir->members = temp;

    // Adiciona informações ao diretório
    for (int i = 0; i < N_novos; i++) {
        append_diretorio(dir, novos[i]);
        dir->N++;
    }

    // Se já havia membros, desloca o conteúdo para abrir espaço
    if (old_N != 0) {
        long deslocamento = N_novos * sizeof(struct infoMember);
        size_t big_chunk = 0;
        for (int i = 0; i < old_N; i++) {
            big_chunk += dir->members[i].diskSize;
        }

        shift_right_archive(archive, dir->members[0].offset, dir->members[0].offset + deslocamento, big_chunk);

        // Escreve novos membros no final do arquivo
        fseek(archive, 0, SEEK_END);
        for (int i = 0; i < N_novos; i++) {
            escreve_membro(archive, novos[i]);
        }

        atualiza_offset(dir);
        escreve_diretorio(archive, dir);

    } else {
        // Caso era um diretório vazio
        atualiza_offset(dir);
        escreve_diretorio(archive, dir);

        fseek(archive, 0, SEEK_END);
        for (int i = 0; i < N_novos; i++) {
            escreve_membro(archive, novos[i]);
        }
    }
}