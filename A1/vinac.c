#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "vinac.h"

void appendMember(FILE* member, FILE* archive){
    
}

void memberInsert(const char *member_name, const char *archive_name){
    FILE* archive = fopen(archive_name, "r+");
    if(!archive){
        archive = fopen(archive_name, "w+");
        if(!archive){
            fprintf(stderr, "Erro ao criar o arquivo archive: %s\n", archive_name);
            return;
        }
    }
    FILE* member = fopen(member_name, "r");
    if(!member){
        fprintf(stderr, "Erro ao abrir o arquivo membro: %s\n", member_name);
        return;
    }        
    appendMember(member, archive);
    fixupArchive(archive);

    fclose(member);
    fclose(archive);
}