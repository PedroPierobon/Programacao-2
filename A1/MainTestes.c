#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "vinac.h"
#include "lz.h"

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

// Cria diretaório e aloca N InfoMembers
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

// Copia o membro iniciando em source de tamanho size para o offset dest
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

// fwrite para escrever o Int N e o vetor de infoMember
void escreve_diretorio(FILE* archive, struct directory* dir) {
    rewind(archive);
    fwrite(&dir->N, sizeof(int), 1, archive); 
    fwrite(dir->members, sizeof(struct infoMember), dir->N, archive);
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

// Retorna um diretório lido dentro do archive
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

// Passa pelo vetor de membros atualizando offset conforme a alocação do vetor
void atualiza_offset(struct directory* dir){
    long new_offset = sizeof(int) + dir->N * sizeof(struct infoMember);
    for(size_t i = 0; i < dir->N; i++){
        dir->members[i].offset = new_offset;
        new_offset += dir->members[i].diskSize;
    }
}

// Escreve na última posição o membro de nome member_name
// Dir deve ter sido alocado espaço para a última posição
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

    free(input);
    free(output);
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

// Insere membro no archive.vc
void memberInsert(FILE* archive, int N_novos, char* novos[], struct directory* dir) {
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

void memberInsert_comp(FILE* archive, int N_novos, char* novos[], struct directory* dir) {
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
    }

    // Escreve membros no final do arquivo
    fseek(archive, 0, SEEK_END);
    for (int i = 0; i < N_novos; i++) {
        long before = ftell(archive);
        escreve_membro_comp(archive, novos[i]);
        long after = ftell(archive);
        dir->members[old_N + i].diskSize = after - before;
    }

    // Agora sim, atualizar offsets e escrever diretório
    atualiza_offset(dir);
    escreve_diretorio(archive, dir);
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

void print_directory_info(struct directory* dir) {
    printf("==== Listando conteúdo do arquivo ====\n");
    for (size_t i = 0; i < dir->N; i++) {
        struct infoMember* m = &dir->members[i];
        printf("==== Membro %zu ====\n", i);
        printf("Nome: %s\n", m->name);
        printf("Tamanho original: %ld bytes\n", m->originalSize);
        printf("Tamanho em disco: %ld bytes\n", m->diskSize);
        printf("Posição no diretório: %d\n", m->pos);
        printf("Última modificação: %s", ctime(&m->modTime));
    }
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

    printf("Extraído: %s\n", membro->name);
    free(buffer);
    free(output);
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


int main(int argc, char *argv[]){
    const char* archive_name = argv[2];
    struct directory* dir;
    // Abre archive
    FILE* archive = fopen(archive_name, "r+");
    if(!archive){
        // Cria archive para append(ponteiro de leitura no começo e escrita no final)
        archive = fopen(archive_name, "w+");
        if(!archive){
            fprintf(stderr, "Erro ao criar o arquivo archive: %s\n", archive_name);
            return 0;
        }
        dir = cria_diretorio(0);
    }else{
        dir = ler_diretorio(archive);
    }

    int N_novos = 0;           // Declare fora do switch
    char **novos = NULL;       // Declare fora do switch

    char option = getopt(argc, argv, "pimxrc");

    switch(option){
        case 'p':
            printf("foi p\n");
            for (int i = 3; i < argc; i++) {
                int idx = encontra_membro(dir, argv[i]);
                if (idx != -1) {
                    substitui_membro(archive, dir, argv[i], idx);
                } else {
                    N_novos++;
                    char **temp = realloc(novos, N_novos * sizeof(char*));
                    if (!temp) {
                        perror("Erro ao realocar novos membros");
                        exit(1);
                    }
                    novos = temp;
                    novos[N_novos - 1] = strdup(argv[i]);
                    if (!novos[N_novos - 1]) {
                        perror("Erro ao duplicar string");
                        exit(1);
                    }
                }
            }
            memberInsert(archive, N_novos, novos, dir);
            for (int i = 0; i < N_novos; i++) {
                free(novos[i]);
            }
            free(novos);
            break;

        case 'i':
            printf("foi i\n");
            for (int i = 3; i < argc; i++) {
                int idx = encontra_membro(dir, argv[i]);
                if (idx != -1) {
                    substitui_membro_comp(archive, dir, argv[i], idx);
                } else {
                    N_novos++;
                    char **temp = realloc(novos, N_novos * sizeof(char*));
                    if (!temp) {
                        perror("Erro ao realocar novos membros");
                        exit(1);
                    }
                    novos = temp;
                    novos[N_novos - 1] = strdup(argv[i]);
                    if (!novos[N_novos - 1]) {
                        perror("Erro ao duplicar string");
                        exit(1);
                    }
                }
            }
            memberInsert_comp(archive, N_novos, novos, dir);
            for (int i = 0; i < N_novos; i++) {
                free(novos[i]);
            }
            free(novos);
            break;

        case 'm':
            printf("foi m\n");
            if(argc < 4){
                perror("Para inserção no ínicio passar NULL como target");
            }else if(argc == 4){
                move_member(archive, argv[3], argv[4], dir, archive_name);
            }else if(argc == 5){
                move_member(archive, argv[3], argv[4], dir, archive_name);
            }
            break;

        case 'x':
            printf("foi x\n");
            if (argc == 3){
                extract(archive, dir, -1, NULL);
            }else{
                extract(archive, dir, argc, argv);
            }
            break;

        case 'r':
            if(argc == 3) {
                while (dir->N > 0) {
                    remove_membro(archive, dir, 0);
                    truncate(archive_name, dir->members[dir->N - 1].offset + dir->members[dir->N - 1].diskSize);
                }
            }
            for (int i = 3; i < argc; i++) {
                int idx = encontra_membro(dir, argv[i]);
                if (idx == -1) {
                    fprintf(stderr, "Membro \"%s\" não encontrado.\n", argv[i]);
                    continue;
                }
            
                remove_membro(archive, dir, idx);
                truncate(archive_name, dir->members[dir->N - 1].offset + dir->members[dir->N - 1].diskSize);
                printf("Removido: %s\n", argv[i]);
            }
            break;

        case 'c':
            printf("foi c\n");
            print_directory_info(dir);
            break;

        default:
            printf("Argumentos: -p -i -m -x -r -c\n");
            return 1;
    }

    free(dir->members);
    free(dir);
    fclose(archive);

    return 0;
}
