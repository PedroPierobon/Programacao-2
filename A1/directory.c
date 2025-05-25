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
#include "lz.h"

struct directory* cria_diretorio(int N) {
    struct directory* dir = malloc(sizeof(struct directory));
    if(!dir){
        perror("Erro ao alocar diretório");
        return NULL;
    }
    
    dir->N = N;
    dir->members = calloc(N, sizeof(struct infoMember));
    if(N != 0 && !dir->members){
        perror("Erro ao alocar vetor de informações");
        return NULL;
    }
    return dir;
}

struct directory* ler_diretorio(FILE* archive){
    rewind(archive);
    int num;
    if (fread(&num, sizeof(int), 1, archive) != 1) {
        perror("Erro ao ler o diretório");
        return NULL;
    }
    struct directory* dir = cria_diretorio(num);
    if (!dir) {
        perror("Erro ao alocar diretório");
        return NULL;
    }
    
    if (fread(dir->members, sizeof(struct infoMember), dir->N, archive)
             != dir->N) {
        perror("Erro ao ler membros do diretório");
        free(dir->members);
        free(dir);
        return NULL;
    }
    
    return dir;
}

int encontra_membro(struct directory* dir, const char* nome) {
    for (size_t i = 0; i < dir->N; i++) {
        if (strcmp(dir->members[i].name, nome) == 0) {
            return i; // retorna o índice do membro
        }
    }
    return -1;
}

void substitui_membro(FILE* archive, struct directory* dir, const char* name, int idx) {
    struct stat statbuf;
    if (stat(name, &statbuf) != 0) {
        perror("Erro ao obter stat");
        return;
    }

    long novo_tam = statbuf.st_size;
    long deslocamento = novo_tam - dir->members[idx].diskSize;

    // Calcula total de bytes após o membro atual
    size_t big_chunk = 0;
    long inicio_deslocamento = dir->members[idx].offset + dir->members[idx].diskSize;

    for (size_t i = idx + 1; i < dir->N; i++) {
        big_chunk += dir->members[i].diskSize;
    }

    // Se for necessário mover, desloca os dados
    if (deslocamento != 0 && big_chunk > 0) {
        if (deslocamento < 0) {
            shift_left_archive(archive,
                inicio_deslocamento,
                inicio_deslocamento + deslocamento,
                big_chunk
            );
        } else {
            shift_right_archive(archive,
                inicio_deslocamento,
                inicio_deslocamento + deslocamento,
                big_chunk
            );
        }

        for (size_t i = idx + 1; i < dir->N; i++) {
            dir->members[i].offset += deslocamento;
        }
    }

    fseek(archive, dir->members[idx].offset, SEEK_SET);
    escreve_membro(archive, name);

    dir->members[idx].diskSize = novo_tam;
    dir->members[idx].modTime = statbuf.st_mtime;
 
    escreve_diretorio(archive, dir);
}

void escreve_diretorio(FILE* archive, struct directory* dir) {
    rewind(archive);
    fwrite(&dir->N, sizeof(int), 1, archive); 
    fwrite(dir->members, sizeof(struct infoMember), dir->N, archive);
}

void append_diretorio(struct directory* dir, const char* member_name){
    struct stat statbuf;
    if (stat(member_name, &statbuf) != 0) {
        perror("Erro ao obter stat");
        return;
    }

    strncpy(dir->members[dir->N].name, member_name, MAX_NAME_SIZE);
    //dir->members[old_N].name = member_name;
    dir->members[dir->N].originalSize = statbuf.st_size;
    dir->members[dir->N].diskSize = statbuf.st_size;
    dir->members[dir->N].modTime = statbuf.st_mtime;
    dir->members[dir->N].pos = dir->N;

    printf("==== Membro %ld adicionado ====\n", dir->N);
    printf("Nome: %s\n", dir->members[dir->N].name);
    printf("Tamanho original: %ld bytes\n", dir->members[dir->N].originalSize);
    printf("Tamanho em disco: %ld bytes\n", dir->members[dir->N].diskSize);
    printf("Posição no diretório: %d\n", dir->members[dir->N].pos);
    printf("Última modificação: %s", ctime(&dir->members[dir->N].modTime));
}

// Passa pelo vetor de membros atualizando offset conforme a alocação do vetor
void atualiza_offset(struct directory* dir){
    long new_offset = sizeof(int) + dir->N * sizeof(struct infoMember);
    for(size_t i = 0; i < dir->N; i++){
        dir->members[i].offset = new_offset;
        new_offset += dir->members[i].diskSize;
    }
}

void substitui_membro_comp(FILE* archive, struct directory* dir, const char* name, int idx) {
    struct stat st;
    if (stat(name, &st) != 0) {
        perror("Erro ao obter stat do arquivo");
        return;
    }

    long original_size = st.st_size;
    time_t mod_time = st.st_mtime;

    FILE* f = fopen(name, "rb");
    if (!f) {
        perror("Erro ao abrir arquivo para leitura");
        return;
    }

    unsigned char* input = malloc(original_size);
    if (!input) {
        perror("Erro ao alocar entrada");
        fclose(f);
        return;
    }
    fread(input, 1, original_size, f);
    fclose(f);

    long max_size = original_size + (original_size / 250) + 1;
    unsigned char* output = malloc(max_size);
    if (!output) {
        perror("Erro ao alocar saída");
        free(input);
        return;
    }

    int compressed_size = LZ_Compress(input, output, original_size);

    // Verifica se a compressão realmente reduziu o tamanho
    if (compressed_size >= original_size) {
        // Se a compressão não reduzir o tamanho, mantém o arquivo original
        compressed_size = original_size;
        memcpy(output, input, original_size);  // Copia o conteúdo original para output sem compressão
    }

    long deslocamento = compressed_size - dir->members[idx].diskSize;

    long pos_inicio = dir->members[idx].offset;
    long pos_fim = pos_inicio + dir->members[idx].diskSize;

    // Atualiza offsets dos membros seguintes
    size_t big_chunk = 0;
    for (size_t i = idx + 1; i < dir->N; i++) {
        big_chunk += dir->members[i].diskSize;
        dir->members[i].offset += deslocamento;
    }

    if (deslocamento < 0) {
        shift_left_archive(archive, pos_fim, pos_fim + deslocamento, big_chunk);
    } else if (deslocamento > 0) {
        shift_right_archive(archive, pos_fim, pos_fim + deslocamento, big_chunk);
    }

    fseek(archive, pos_inicio, SEEK_SET);
    fwrite(output, 1, compressed_size, archive);

    
    dir->members[idx].diskSize = compressed_size;
    dir->members[idx].originalSize = original_size;
    dir->members[idx].modTime = mod_time;

    escreve_diretorio(archive, dir);

    free(input);
    free(output);
}

void append_diretorio_comp(struct directory* dir, const char* filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("Erro ao obter informações do arquivo");
        return;
    }

    long original_size = st.st_size;
    time_t mod_time = st.st_mtime;

    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Erro ao abrir arquivo para leitura");
        return;
    }

    unsigned char* input = malloc(original_size);
    if (!input) {
        perror("Erro ao alocar memória para input");
        fclose(f);
        return;
    }

    fread(input, 1, original_size, f);
    fclose(f);

    long max_compressed_size = original_size + (original_size / 250) + 1;
    unsigned char* output = malloc(max_compressed_size);
    if (!output) {
        perror("Erro ao alocar memória para output");
        free(input);
        return;
    }

    int compressed_size = LZ_Compress(input, output, original_size);

    // Decide se guarda comprimido ou não
    int use_compressed = compressed_size < original_size;

    struct infoMember novo;
    snprintf(novo.name, sizeof(novo.name), "%s", filename);
    novo.originalSize = original_size;
    novo.diskSize = use_compressed ? compressed_size : original_size;
    novo.modTime = mod_time;
    novo.pos = dir->N;
    novo.offset = -1; // será definido depois por atualiza_offset()

    dir->members[dir->N] = novo;

    printf("==== Membro %ld comprimido adicionado ====\n", dir->N);
    printf("Nome: %s\n", dir->members[dir->N].name);
    printf("Tamanho original: %ld bytes\n", dir->members[dir->N].originalSize);
    printf("Tamanho em disco: %ld bytes\n", dir->members[dir->N].diskSize);
    printf("Posição no diretório: %d\n", dir->members[dir->N].pos);
    printf("Última modificação: %s", ctime(&dir->members[dir->N].modTime));

    free(input);
    free(output);
}