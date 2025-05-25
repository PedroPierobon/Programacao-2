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

int main(int argc, char *argv[]){
    const char* archive_name = argv[2];
    struct directory* dir = NULL;

    FILE* archive = abre_arquivo(archive_name, &dir);

    int N_novos = 0;    
    char **novos = NULL;

    char option = getopt(argc, argv, "pimxrc");

    switch(option){
        case 'p':
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
            insere(archive, N_novos, novos, dir);
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
            insere_comp(archive, N_novos, novos, dir);
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
            printar(dir);
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

