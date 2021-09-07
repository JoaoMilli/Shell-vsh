#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void handle_sigforeground(int sig){
    printf("Processo foreground\n");
    return;
}

void handle_sigbackground(int sig){
    printf("Cof Cof...\n");
    raise(SIGINT);
}

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

void execute(char **argv, int background, int* prim, pid_t *grupo)
{
    pid_t  pid;
    int status;


    if ((pid = fork()) < 0) {     /* fork a child process           */
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0) {          /* for the child process:         */
        if(!background){
            signal(SIGUSR1, SIG_IGN);
            signal(SIGUSR2, SIG_IGN);
        }
        else{
            signal(SIGUSR1, handle_sigbackground);
            signal(SIGUSR2, handle_sigbackground);
        }
        if (execvp(*argv, argv) < 0) {     /* execute the command  */
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    }
    else if (pid > 0){                                  /* for the parent:      */
        printf("Criado filho com PID: %d\n", pid);
        if(!(*prim)){
            setpgid(pid,pid);
            *grupo = pid;
        }
        else{
            setpgid(pid,(*grupo));
        }
        (*prim)++;
        
        pid_t gpid = getpgid(pid);
        printf("Filho com GPID: %d\n", gpid);

        if(!background){
            while (wait(&status) != pid);   /* wait for completion  */
        }

    }
    else return;
}

void handleSIGCHLD(int sig){
    pid_t pid = waitpid(-1, NULL, WNOHANG);
    kill(-pid, SIGUSR1);
}

void handleSIGUSR(int sig){
    printf("Sinal");
}

void  main(void)
{
    char  line[1024];             /* the input line                 */
    char  *argv[64];              /* the command line argument      */
    char comandos[5][200];
    int prim;
    pid_t grupo;

    signal(SIGCHLD, handleSIGCHLD);
    signal(SIGUSR1, handleSIGUSR);
    signal(SIGUSR2, handleSIGUSR);

    while (1) {                   /* repeat until done ....         */
        prim = 0;
        sleep(1);
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

        if(nComandos == 1){
            parse(comandos[0], argv);
            execute(argv,0, &prim, &grupo);
        }
        else{
            for(int k=0; k<nComandos; k++){
                parse(comandos[k], argv);
                execute(argv,1, &prim, &grupo);
            }
        }

    }
}