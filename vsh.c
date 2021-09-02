#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    const char s[2] = " ";
    int nComandos = 0;
    int nArgs[5];
    char** comandos = malloc(sizeof(char*) * 5);
    char** argumentos = malloc(sizeof(char*) * 5);
    char* linha = malloc(sizeof(char) * 201);
    char* token = malloc(sizeof(char)*101);

    while(1){

        for(int i=0; i<4; i++){
            nArgs[i] = 0;
        }

        printf("vsh> ");
        fgets(linha, 200, stdin);
        nComandos = 1;

        token = strtok(linha, s);
        comandos[0] = strdup(token);
        token = strtok(NULL, s);

        while(token != NULL){
        
            if(!strcmp(token, "|")){
                token = strtok(NULL, s);
                comandos[nComandos] = strdup(token);
                nComandos++;
            }
            else{
                printf("Args");
                argumentos[nArgs[nComandos]] = strdup(token);
                nArgs[nComandos]++;
            }

            token = strtok(NULL, s);
        }

    }

    free(token);

    return 0;
}