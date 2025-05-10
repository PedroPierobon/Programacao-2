#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "vinac.h"

// Escreve Membro no ponteiro onde archive está
void escreve_membro(FILE* archive, const char *member_name){
    // Escrever membro
    char buffer[MAX_BUFFER_SIZE];
    FILE* member = fopen(member_name, "r");
    if(!member){
        perror("Erro ao abrir o membro");
        return;
    }
    
    size_t bytes;
    while((bytes = fread(buffer, 1, sizeof(buffer), member)) > 0) {
        fwrite(buffer, 1, bytes, archive);
    }
    fclose(member);
}

// Cria diretório e aloca N InfoMembers
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


void shift_right_archive(FILE* archive, long source, long dest, size_t size){
    if(size == 0 || dest <= source) return;
    
    char buffer[MAX_BUFFER_SIZE];
    size_t bytes = size;

    while(bytes > 0){
        // move com blocos de tamanho block
        // se o tamanho for menor que o buffer só preenche
        // esse tamanho faltante
        size_t chunk = (bytes >= MAX_BUFFER_SIZE) ? MAX_BUFFER_SIZE : bytes;
        
        long read_pos = source + bytes - chunk;
        long write_pos = dest + bytes - chunk;
        
        fseek(archive, read_pos, SEEK_SET);
        if (fread(buffer, 1, chunk, archive) != chunk) {
            perror("Erro ao ler dados no shift");
            return;
        }
        
        fseek(archive, write_pos, SEEK_SET);
        if (fwrite(buffer, 1, chunk, archive) != chunk) {
            perror("Erro ao escrever dados no shift");
            return;
        }

        bytes -= chunk;
    }
}

void shift_left_archive(FILE* archive, long source, long dest,  size_t size){
    if (size == 0 || dest >= source) return;

    char buffer[MAX_BUFFER_SIZE];
    size_t bytes = 0;

    while (bytes < size) {
        size_t chunk = (size - bytes >= MAX_BUFFER_SIZE) ? MAX_BUFFER_SIZE : (size - bytes);

        long read_pos = source + bytes;
        long write_pos = dest + bytes;

        fseek(archive, read_pos, SEEK_SET);
        if (fread(buffer, 1, chunk, archive) != chunk) {
            perror("Erro ao ler dados no shift left");
            return;
        }

        fseek(archive, write_pos, SEEK_SET);
        if (fwrite(buffer, 1, chunk, archive) != chunk) {
            perror("Erro ao escrever dados no shift left");
            return;
        }

        bytes += chunk;
    }
}

void escreve_diretorio(FILE* archive, struct directory* dir) {
    rewind(archive);
    fwrite(dir, sizeof(struct directory), 1, archive); 
    fwrite(dir->members, sizeof(struct infoMember), dir->N, archive);
}

struct directory* ler_diretorio(FILE* archive){
    rewind(archive);
    struct directory* dir = cria_diretorio(0);
    if (!dir) {
        perror("Erro ao alocar diretório");
        return NULL;
    }
    
    if (fread(dir, 1, sizeof(struct directory), archive) != sizeof(struct directory)) {
        perror("Erro ao ler o diretório");
        free(dir);
        return NULL;
    }
    
    dir->members = calloc(dir->N, sizeof(struct infoMember));
    if (dir->N != 0 && !dir->members) {
        perror("Erro ao alocar vetor de membros");
        free(dir);
        return NULL;
    }
    
    if (fread(dir->members, sizeof(struct infoMember), dir->N, archive) != dir->N) {
        perror("Erro ao ler membros do diretório");
        free(dir->members);
        free(dir);
        return NULL;
    }
    
    return dir;
}

// NÃO COLOCA O OFFSET
void append_diretorio(struct directory* dir, const char* member_name, int old_N){
    struct stat statbuf;
    stat(member_name, &statbuf);
    dir->members[old_N].name = member_name;
    dir->members[old_N].originalSize = statbuf.st_size;
    dir->members[old_N].diskSize = statbuf.st_size;
    dir->members[old_N].modTime = statbuf.st_mtime;
    dir->members[old_N].pos = old_N;
}

void memberInsert(FILE* archive, int argc, char* argv[], struct directory* dir){
    int qtd_membros = argc - 3;
    int old_N = dir->N;
    int new_N = dir->N + qtd_membros;
    
    if(!(dir->members = realloc(dir->members, new_N * sizeof(struct infoMember)))){
        perror("Erro ao realocar vetor de infoMember");
        return;
    }
    
    for (int i = 3; i < argc; i++){
        append_diretorio(dir, argv[i], dir->N);
        dir->N++;
    }

    if(old_N != 0){
        long deslocamento = qtd_membros * sizeof(struct infoMember);
        size_t big_chunk = 0;
        for(int i = 0; i < old_N; i++){
            big_chunk += dir->members[i].diskSize;
        }
        shift_right_archive(archive, dir->members[0].offset, dir->members[0].offset + deslocamento, big_chunk);
    }


    free(dir->members);
    free(dir);
    fclose(archive);
}

int main(int argc, char *argv[]){
    const char* archive_name = argv[2];
    struct directory* dir;
    //argv[0] = ./vinac
    //argv[1] = -i
    //argv[2] = meu.vc
    int qtd_membros = argc - 3;
    // Abre archive
    FILE* archive = fopen(archive_name, "r+");
    if(!archive){
        // Cria archive para append(ponteiro de leitura no começo e escrita no final)
        archive = fopen(archive_name, "w+");
        if(!archive){
            fprintf(stderr, "Erro ao criar o arquivo archive: %s\n", archive_name);
            return;
        }
        dir = cria_diretorio(qtd_membros);
    }else{
        dir = ler_diretorio(archive);
    }
    memberInsert(archive, argc, argv, dir);
    
}
