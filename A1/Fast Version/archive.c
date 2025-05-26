#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "archive.h"
#include "directory.h"
#include "lz.h"

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

void extrai_membro(FILE* archive, struct infoMember* membro) {
    unsigned char* buffer = malloc(membro->diskSize);
    unsigned char* output = malloc(membro->originalSize);

    if (!buffer || !output) {
        perror("Erro ao alocar memória");
        free(buffer);
        free(output);
        return;
    }

    fseek(archive, membro->offset, SEEK_SET);
    fread(buffer, 1, membro->diskSize, archive);

    if (membro->diskSize < membro->originalSize) {
        LZ_Uncompress(buffer, output, membro->diskSize);
    } else {
        memcpy(output, buffer, membro->originalSize);
    }

    FILE* out = fopen(membro->name, "wb");
    if (!out) {
        perror("Erro ao criar arquivo extraído");
        free(buffer);
        free(output);
        return;
    }

    fwrite(output, 1, membro->originalSize, out);
    fclose(out);
    
    free(buffer);
    free(output);
}

void escreve_membro_comp(FILE* archive, const char* filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("Erro ao obter informações do arquivo");
        return;
    }

    long original_size = st.st_size;

    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Erro ao abrir arquivo");
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

    if (compressed_size < original_size) {
        fwrite(output, 1, compressed_size, archive);
    } else {
        fwrite(input, 1, original_size, archive);
    }

    free(input);
    free(output);
}