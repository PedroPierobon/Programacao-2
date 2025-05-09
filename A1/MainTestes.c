#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

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

struct directory* cria_diretorio() {
    struct directory* dir = malloc(sizeof(struct directory));
    dir->N = 0;
    dir->members = NULL;
    return dir;
}

void memberInsert(const char *member_name, const char *archive_name){
    // Abre archive
    FILE* archive = fopen(archive_name, "r+");
    if(!archive){
        // Cria archive para append(ponteiro de leitura no começo e escrita no final)
        archive = fopen(archive_name, "w+");
        if(!archive){
            fprintf(stderr, "Erro ao criar o arquivo archive: %s\n", archive_name);
            return;
        }
        
        escreve_membro(archive, member_name);
        
        
        // Atualizar diretório
        // Criar Diretório
        struct directory* dir = cria_diretorio();
        if(!dir){
            perror("Erro ao alocar o diretório");
            fclose(archive);
            return;
        }
        
        // Atualizar Diretório
        dir->N = 1;
        dir->members = malloc(dir->N * sizeof(struct infoMember));
        
        /////////////////////////////////
        // COLOCAR INFOMAÇÕES DOS MEMBROS
        /////////////////////////////////

        // Mover Membro para abrir espaço
        long deslocamento = sizeof(struct directory) + dir->N * sizeof(struct infoMember);

        rewind(archive);
        char* temp = malloc(tam_membro);
        fread(temp, 1, tam_membro, archive);

        fseek(archive, deslocamento, SEEK_SET); // setando o ponteiro no offset do começo
        fwrite(temp, 1, tam_membro, archive);
        free(temp);


        rewind(archive);
        fwrite(dir, sizeof(struct directory), 1, archive); 
        fwrite(dir->members, sizeof(struct infoMember), dir->N, archive);

        free(dir->members);
        free(dir);
        fclose(archive);
    }
}

int main(int argc, char *argv[]){
    const char* archive = argv[2];
    memberInsert(argv[3], archive);
    
}