#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void  parse(char *line, char **argv)
{
    while (*line != '\0') {       
        while (*line == ' ' || *line == '\t' || *line == '\n'){
            *line++ = '\0';  
        }
        if(*line != '\0'){
            *argv++ = line; 
        }         
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n'){
            line++;  
        }            
    }
    *argv = '\0';                 
}

void execute(char **argv)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {     /* fork a child process           */
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0) {          /* for the child process:         */
        if (execvp(*argv, argv) < 0) {     /* execute the command  */
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    }
    else {                                  /* for the parent:      */
        while (wait(&status) != pid);   /* wait for completion  */
    }
}

void  main(void)
{
     char  line[1024];             /* the input line                 */
     char  *argv[64];              /* the command line argument      */
     char comandos[5][200];

    while (1) {                   /* repeat until done ....         */
        printf("Shell -> ");     /*   display a prompt             */
        fgets(line, 200, stdin);             /*   read in the command line     */
        printf("\n");

        int i=0, j=0;
        int nComandos = 0;

        while(line[i] != '\0'){
            nComandos++;
            while(line[i] != '\0' && line[i] != '|'){
                comandos[nComandos-1][i] = line[i];
                i++;
            }
            comandos[nComandos-1][i] = '\0';
            if(line[i] == '|'){
                nComandos++;
                i++;
                while(line[i] != '\0' && line[i] != '|'){
                    comandos[nComandos-1][j] = line[i];
                    i++;
                    j++;
                }
                comandos[nComandos-1][j] = '\0';
                if(line[i] == '|'){
                    nComandos++;
                    j=0;
                    i++;
                    while(line[i] != '\0' && line[i] != '|'){
                        comandos[nComandos-1][j] = line[i];
                        i++;
                        j++;
                    }
                    comandos[nComandos-1][j] = '\0';
                    if(line[i] == '|'){
                        nComandos++;
                        j=0;
                        i++;
                        while(line[i] != '\0' && line[i] != '|'){
                            comandos[nComandos-1][j] = line[i];
                            i++;
                            j++;
                        }
                        comandos[nComandos-1][j] = '\0';
                        if(line[i] == '|'){
                            nComandos++;
                            j=0;
                            i++;
                            while(line[i] != '\0' && line[i] != '|'){
                                comandos[nComandos-1][j] = line[i];
                                i++;
                                j++;
                            }
                            comandos[nComandos-1][j] = '\0';
                        }          
                    }
                }
            }
        }

        for(int k=0; k<nComandos; k++){
            parse(comandos[k], argv);
            execute(argv);
        }
     }
}