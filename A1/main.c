#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

void memberInsert(const char *member_name, const char *archive_name){
    return;
}

int main(int argc, char *argv[]){
    char option;
    const char *archive = argv[2];
    while((option = getopt(argc, argv, "pimxrc")) != -1){
        switch(option){
            case 'p':
                printf("foi p\n");
                for(int i = optind + 1; i < argc; i++){
                    memberInsert(argv[i], archive);
                }
                break;
            case 'i':
                printf("foi i\n");
                break;
            case 'm':
                printf("foi m\n");
                break;
            case 'x':
                printf("foi x\n");
                break;
            case 'r':
                printf("foi r\n");
                break;
            case 'c':
                printf("foi c\n");
                break;
            default:
                printf("Argumentos: -p -i -m -x -r -c\n");
                return 1;
        }
    }
    
    return 0;
}