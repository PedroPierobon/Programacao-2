#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "archive.h"
#include "directory.h"

FILE* abre_arquivo (const char* archive_name, struct directory** dir) {
    FILE* archive = fopen(archive_name, "r+");
    if(!archive){
        // Cria archive para append(ponteiro de leitura no começo e escrita no final)
        archive = fopen(archive_name, "w+");
        if(!archive){
            fprintf(stderr, "Erro ao criar o arquivo archive: %s\n", archive_name);
            return 0;
        }
        *dir = cria_diretorio(0);
    }else{
        *dir = ler_diretorio(archive);
    }
    return archive;
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

// Copia o membro iniciando em source de tamanho size para o offset dest
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