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
#include "lz.h"

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

void insere_comp(FILE* archive, int N_novos, char* novos[], struct directory* dir) {
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

    // Adiciona informações ao diretório (sem atualizar offset ainda)
    for (int i = 0; i < N_novos; i++) {
        append_diretorio_comp(dir, novos[i]);
        dir->N++;
    }

    // Se já havia membros, desloca para abrir espaço pro novo diretório
    if (old_N != 0) {
        long deslocamento = N_novos * sizeof(struct infoMember);
        size_t big_chunk = 0;
        for (int i = 0; i < old_N; i++) {
            big_chunk += dir->members[i].diskSize;
        }

        shift_right_archive(archive, dir->members[0].offset, dir->members[0].offset + deslocamento, big_chunk);
        
        // Escreve membros no final do arquivo
        fseek(archive, 0, SEEK_END);
        for (int i = 0; i < N_novos; i++) {
            long before = ftell(archive);
            escreve_membro_comp(archive, novos[i]);
            long after = ftell(archive);
            dir->members[old_N + i].diskSize = after - before;
        }

        atualiza_offset(dir);
        escreve_diretorio(archive, dir);
    }else{
        // Escreve o Diretório antes caso arquivo vazio
        atualiza_offset(dir);
        escreve_diretorio(archive, dir);

        // Escreve membros no final do arquivo
        fseek(archive, 0, SEEK_END);
        for (int i = 0; i < N_novos; i++) {
            long before = ftell(archive);
            escreve_membro_comp(archive, novos[i]);
            long after = ftell(archive);
            dir->members[old_N + i].diskSize = after - before;
        }
    }
}

void move_member(FILE* archive, const char* member, const char* target, struct directory* dir, const char* archive_name) {
    // Procura o id dos membros buscados
    ssize_t id_m = -1, id_t = -1;
    for (ssize_t i = 0; (size_t)i < dir->N; i++) {
        if (strcmp(dir->members[i].name, member) == 0) id_m = i;
        if (target && strcmp(dir->members[i].name, target) == 0) id_t = i;
    }

    if (!target) {
        if (id_m == 0) {
            fprintf(stderr, "Membro já está na posição inicial. Não houve mudança\n");
            return;
        }
    
        struct infoMember info_m = dir->members[id_m];
    
        // Copia membro para o fim
        fseek(archive, 0, SEEK_END);
        long fim_arquivo = ftell(archive);
        shift_right_archive(archive, info_m.offset, fim_arquivo, info_m.diskSize);

        long inicio_membros = dir->members[0].offset;
    
        // Move os outros para a direita
        size_t big_chunk = 0;
        long curr_offset = info_m.offset + info_m.diskSize;
        for (ssize_t i = id_m - 1; i >= 0; i--) {
            big_chunk += dir->members[i].diskSize;
            dir->members[i].pos++;
            dir->members[i + 1] = dir->members[i];
            curr_offset -= dir->members[i + 1].diskSize;
            dir->members[i + 1].offset = curr_offset;
        }
        shift_right_archive(archive, inicio_membros, inicio_membros + info_m.diskSize, big_chunk);
    
        // Coloca info_m na posição 0
        dir->members[0] = info_m;
        dir->members[0].pos = 0;
        dir->members[0].offset = inicio_membros;
        shift_left_archive(archive, fim_arquivo, inicio_membros, info_m.diskSize);
    
        truncate(archive_name, fim_arquivo);
        escreve_diretorio(archive, dir);
        return;
    }
    
    if (id_m == -1) {
        fprintf(stderr, "Membro '%s' não encontrado\n", member);
        return;
    }

    if (target && id_t == -1) {
        fprintf(stderr, "Target '%s' não encontrado\n", target);
        return;
    }

    if (target && id_t == id_m) {
        fprintf(stderr, "Membro e target são iguais. Não houve mudança\n");
        return;
    }

    if (id_m == id_t + 1){
        fprintf(stderr, "Membro está na posição correta. Não houve mudança\n");
        return;
    }

    struct infoMember info_m = dir->members[id_m];
    struct infoMember info_t = dir->members[id_t];

    // Copiar arquivo para o final 
    long fim_arquivo;
    fseek(archive, 0, SEEK_END);
    fim_arquivo = ftell(archive);
    shift_right_archive(archive, info_m.offset, fim_arquivo, info_m.diskSize);

    // Se o info_m está antes do target
    if(id_m < id_t){
        // Mover de id_m + 1 até target para a esquerda
        size_t big_chunk = 0;
        long curr_offset = info_m.offset;
        for(ssize_t i = id_m + 1; i <= id_t; i++){
            // calcula tamanho que vai ser shiftado
            big_chunk += dir->members[i].diskSize;
            // atualiza pos do diretório
            dir->members[i].pos--;
            dir->members[i - 1] = dir->members[i];
            dir->members[i - 1].offset = curr_offset;
            curr_offset += dir->members[i - 1].diskSize;
        }
        shift_left_archive(archive, info_m.offset + info_m.diskSize, info_m.offset, big_chunk);

        // volta o M para a posição do target
        long novo_offset = dir->members[id_t - 1].offset + dir->members[id_t - 1].diskSize;
        dir->members[id_t] = info_m;
        dir->members[id_t].pos = id_t;
        dir->members[id_t].offset = novo_offset;
        shift_left_archive(archive, fim_arquivo, novo_offset, info_m.diskSize);
    }else{
        // Mover de id_t + 1 até id_m - 1 para a esquerda
        size_t big_chunk = 0;
        long curr_offset = dir->members[id_m + 1].offset;
        for(ssize_t i = id_m - 1; i > id_t; i--){
            // calcula tamanho que vai ser shiftado
            big_chunk += dir->members[i].diskSize;
            // atualiza pos do diretório
            dir->members[i].pos++;
            dir->members[i + 1] = dir->members[i];
            curr_offset -= dir->members[i + 1].diskSize;
            dir->members[i + 1].offset = curr_offset;
        }
        shift_right_archive(archive, info_t.offset + info_t.diskSize, dir->members[id_t + 2]. offset, big_chunk);

        // volta o M para a posição do target
        long novo_offset = info_t.offset + info_t.diskSize;
        dir->members[id_t + 1] = info_m;
        dir->members[id_t + 1].pos = id_t + 1;
        dir->members[id_t + 1].offset = novo_offset;
        shift_left_archive(archive, fim_arquivo, novo_offset, info_m.diskSize);
    }
    escreve_diretorio(archive, dir);
    truncate (archive_name, fim_arquivo);
}

void extract(FILE* archive, struct directory* dir, int argc, char* argv[]) {
    if (argv == NULL) {
        // Extrair todos
        for (size_t i = 0; i < dir->N; i++) {
            extrai_membro(archive, &dir->members[i]);
        }
    } else {
        // Extrair membros específicos
        for (int j = 3; j < argc; j++) {
            int encontrado = 0;
            for (size_t i = 0; i < dir->N; i++) {
                if (strcmp(argv[j], dir->members[i].name) == 0) {
                    extrai_membro(archive, &dir->members[i]);
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                fprintf(stderr, "Membro \"%s\" não encontrado no arquivo.\n", argv[j]);
            }
        }
    }
}

void remove_membro(FILE* archive, struct directory* dir, size_t idx){
    struct infoMember target = dir->members[idx];
    // Membros a direita shiftam a esquerda dir.members[idx].diskSize
    size_t tam = 0;
    for(size_t i = idx + 1; i < dir->N; i++){
        tam += dir->members[i].diskSize;
        dir->members[i].pos = i - 1;
        dir->members[i - 1] = dir->members[i];
    }
    shift_left_archive(archive, target.offset + target.diskSize, target.offset, tam);

    // Realloc do vetor dos infoMembers e atualização de posições
    dir->N--;
    struct infoMember* temp = realloc(dir->members, dir->N * sizeof(struct infoMember));
    if (!temp) {
        perror("Erro ao realocar vetor de infoMember");
        return;
    }
    dir->members = temp;

    for(size_t i = 0; i < idx; i++){
        tam += dir->members[i].diskSize;
    }
    
    // Shift Left de todos os membros 1 * sizeof(struct infoMember)
    shift_left_archive(archive, dir->members[0].offset, dir->members[0].offset - sizeof(struct infoMember), tam);
    // Atualização de offsets no dir
    atualiza_offset(dir);
    // Escreve dir
    escreve_diretorio(archive, dir);
}

void printar(struct directory* dir) {
    printf("==== Listando conteúdo do arquivo ====\n");
    for (size_t i = 0; i < dir->N; i++) {
        struct infoMember* m = &dir->members[i];
        printf("==== Membro %zu ====\n", i);
        printf("Nome: %s\n", m->name);
        printf("Offset: %ld\n", m->offset);
        printf("Tamanho original: %ld bytes\n", m->originalSize);
        printf("Tamanho em disco: %ld bytes\n", m->diskSize);
        printf("Posição no diretório: %d\n", m->pos);
        printf("Última modificação: %s", ctime(&m->modTime));
    }
}