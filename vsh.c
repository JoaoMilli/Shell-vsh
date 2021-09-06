#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void parse(char *line, char **argv)
{
    while (*line != '\0') {                                                                         /* if not the end of line ....... */ 
        while (*line == ' ' || *line == '\t' || *line == '\n') *line++ = '\0';                      /* replace white spaces with 0    */
        *argv++ = line;                                                                             /* save the argument position     */
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') line++;             /* skip the argument until ...    */
    }
    *argv = '\0';                                                                                  /* mark the end of argument list  */
}


int main(){

    const char s[2] = " ";
    int nComandos = 0;
    int nArgs[5];
    char** comandos = malloc(sizeof(char*) * 5);
    char** argumentos = malloc(sizeof(char*) * 5);;
    char* linha = malloc(sizeof(char) * 201);
    char* token = malloc(sizeof(char)*101);

    char* linhaArg = malloc(sizeof(char) * 201);

    while(1){

        for(int i=0; i<4; i++){
            nArgs[i] = 0;
        }

        printf("vsh> ");
        fgets(linha, 200, stdin);

        token = strtok(linha, s);
        comandos[0] = strdup(token);
        if(comandos[0] != NULL) nComandos = 1;
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

        if(nComandos == 1){
            
        }

        printf("\n");
    }

    free(token);

    return 0;
}