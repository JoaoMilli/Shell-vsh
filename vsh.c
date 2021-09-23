#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

/*  

Função que preenche o vetor argv de acordo com o padrão recebido pelo exec de acordo com o comando contido no vetor linha

Inputs: String linha contendo o comando completo a ser executado e ponteiro para string argv que receberá os argumentos no formato padrão recebido pelo exec
Output: Nada

*/

void parse(char *linha, char **argv);            

/*  

Função que cria um novo processo para que um comando seja executado

Inputs: O ponteiro para string argv contendo os argumentos do comando a ser executado no formato padrão recebido pelo exec, 
Um inteiro background contendo o booleano representando se o processo será ou não executado em background
Um ponteiro para um valor grupo contendo o group id da sessão

Output: Nada

*/

void execute(char **argv, int background, int* prim, pid_t *grupo, pid_t *processos, int* nProcessos);

/*  

Função que substituirá o tratador padrão de SIGCHLD

Inputs: O sinal a ser tratado

Output: Nada

*/

void handleSIGCHLD(int sig);

/*  

Função que substituirá o tratador padrão de SIGUSR1 e SIGUSR2

Inputs: O sinal a ser tratado

Output: Nada

*/

void handleSIGUSR(int sig);

int main(void) {

    char linha[1024];             // String contendo o comando completo enviado pelo shell
    char *argv[64];               // String contendo os argumentos a serem executados no formato que é recebido pelo exec
    char comandos[5][200];         // Vetor de strings contendo todos os comandos completos enviados pelo shell
    int prim;                      // Booleano que representa se um comando é o primeiro de uma sessão
    pid_t grupo;                   // Valor do group id da sessão
    pid_t* processos = malloc(sizeof(pid_t)*100);
    int nProcessos = 0;

    /*Instala tratadores específicos para os sinais SIGCHLD, SIGUSR1, SIGUSR2*/

    signal(SIGCHLD, handleSIGCHLD);
    signal(SIGUSR1, handleSIGUSR);
    signal(SIGUSR2, handleSIGUSR);

    /*O shell entra em um loop infinito até receber um sinal que mata seu processo*/

    while (1) {                   
        prim = 0;                              // Atribui o valor de prim para 0 em cada sessão, para que a cada nova sessão um novo group id seja atribuido para os processos
        sleep(1);                              // O shell espera 1 segundo antes de receber um novo input, para sicronizar os printfs dos processos filhos
        printf("vsh-> ");                      
        fgets(linha, 200, stdin);
        printf("\n");

        int i=0, j=0;
        int nComandos = 0;

        /*Após receber todos os comandos na string linha, a string de cada comando é separada em cada index do vetor comandos*/

        while(linha[i] != '\0'){
            nComandos++;
            while(linha[i] != '\0' && linha[i] != '|'){
                comandos[nComandos-1][i] = linha[i];
                i++;
            }
            comandos[nComandos-1][i] = '\0';
            if(linha[i] == '|'){
                nComandos++;
                i++;
                while(linha[i] != '\0' && linha[i] != '|'){
                    comandos[nComandos-1][j] = linha[i];
                    i++;
                    j++;
                }
                comandos[nComandos-1][j] = '\0';
                if(linha[i] == '|'){
                    nComandos++;
                    j=0;
                    i++;
                    while(linha[i] != '\0' && linha[i] != '|'){
                        comandos[nComandos-1][j] = linha[i];
                        i++;
                        j++;
                    }
                    comandos[nComandos-1][j] = '\0';
                    if(linha[i] == '|'){
                        nComandos++;
                        j=0;
                        i++;
                        while(linha[i] != '\0' && linha[i] != '|'){
                            comandos[nComandos-1][j] = linha[i];
                            i++;
                            j++;
                        }
                        comandos[nComandos-1][j] = '\0';
                        if(linha[i] == '|'){
                            nComandos++;
                            j=0;
                            i++;
                            while(linha[i] != '\0' && linha[i] != '|'){
                                comandos[nComandos-1][j] = linha[i];
                                i++;
                                j++;
                            }
                            comandos[nComandos-1][j] = '\0';
                        }          
                    }
                }
            }
        }

        /*Se houver apenas um comando no input, é feito o parsing do comando preenchendo o vetor argv de acordo com o padrão recebido pelo exec, e o comando é executado em foreground*/
        /*O group id dos processos da sessão é setado como o pid do primeiro comando da sessão*/

        if(nComandos == 1){

            /*Comando liberamoita (comando interno)*/

            if (!strcmp(comandos[0], "liberamoita\n")){
                pid_t wpid;

                /*invoca waitpid ate que nao exista mais processos terminados, isso recolhe os status dos processos filhos acabados e terminam seu estado zombie*/
                while((wpid = waitpid(-1,NULL,WNOHANG)) > 0);
                printf("\nProcessos zumbis liberados\n");
            }

            /*Comando armageddon (comando interno)*/

            else if (!strcmp(comandos[0], "armageddon\n")) {
                printf("\nTerminando todos os processos e encerrando terminal\n");

                /*Envia o sinal SIGINT para todos os processos criados pelo shell, causando sua terminacao*/
                for (int i=0; i<nProcessos; i++){
                    kill(processos[i], SIGINT);
                }

                free(processos);
                return 0;
                
            }

            /*Comando externo*/

            else{
                parse(comandos[0], argv);
                execute(argv,0, &prim, &grupo, processos, &nProcessos);
            }
        }
        /*Se houver mais de um comando no input, é feito o parsing de cada comando preenchendo o vetor argv de acordo com o padrão recebido pelo exec, e cada comando é executado em background*/
        /*O group id dos processos da sessão é setado como o pid do primeiro comando da sessão*/

        else{
            for(int k=0; k<nComandos; k++){
                parse(comandos[k], argv);
                execute(argv,1, &prim, &grupo, processos, &nProcessos);
            }
        }

    }
}

void parse (char *linha, char **argv) {

    /*Verifica se o caractere da linha contendo o comando é /0 , em caso positivo o parsing está completo*/
    
    while (*linha != '\0') {  

        /*Preence os espaços da linha com /0 até encontrar um caractere válido*/

        while (*linha == ' ' || *linha == '\t' || *linha == '\n'){
            *linha++ = '\0';  
        }

        /*Caso o próximo caractere válido não seja um /0, argv recebe o ponteiro para esse novo caractere e incrementa seu index para que receba outro ponteiro*/

        if(*linha != '\0'){
            *argv++ = linha; 
        } 

        /*Incrementa o index de linha até que seja encontrado um novo caractere inválido*/

        while (*linha != '\0' && *linha != ' ' && *linha != '\t' && *linha != '\n'){
            linha++;  
        }            
    }

    /*Termina o parsing atribuindo /0 ao final de argv*/

    /*No final das contas, argv será um vetor de ponteiros com cada índice apontando para o primeiro caractere de cada argumento, separados por /0 */

    *argv = '\0';                 
}

void execute(char **argv, int background, int* prim, pid_t *grupo, pid_t *processos, int* nProcessos) {

    pid_t pid;
    int status;

    /*Faz o fork e armazena o valor do pid do filho em pid*/

    if ((pid = fork()) < 0) {   
        printf("ERRO: fork falhou!\n");
        exit(1);
    }

    /*Código do processo filho*/

    else if (pid == 0) {

        /*Se o processo for executado em foreground, ignora os sinais SIGUSR1 e SIGUSR2*/

        if(!background){
            signal(SIGUSR1, SIG_IGN);
            signal(SIGUSR2, SIG_IGN);
        }

        /*Executa o comando em argv*/

        if (execvp(*argv, argv) < 0) {     
            printf("ERRO: exec falhou!\n");
            exit(1);
        }
    }

    /*Código do processo pai*/

    else if (pid > 0) {
        processos[*nProcessos] = pid;
        (*nProcessos)++;

        /* Se for o primeiro comando da sessão, seta o seu group id como sendo o próprio pid, o pid desse processo será o group id da sessão*/

        if(!(*prim)){
            
            setpgid(pid,pid);
            *grupo = pid;
        }

        /*Caso não seja o primeiro comando da sessão, seta o seu group id como o pid do primeiro comando da sessão*/

        else{
            setpgid(pid,(*grupo));
        }

        (*prim)++;

        /*Se o processo for foreground, o processo do shell deve bloquear e aguardar até que o processo filho tenha terminado*/

        if(!background){
            while (wait(&status) != pid); 
        }

    }
    else return;
}

/*Implementação do tratador do sinal SIGCHLD*/

void handleSIGCHLD(int sig){

    /*Quando qualquer filho do shell terminar sua execução, o pai receberá o sinal SIGCHLD e a rotina desse tratador será utilizada*/
    int status;

    /*Obtém-se o valor do pid do processo filho encerrado e informações de status*/
    pid_t pid = waitpid(-1, &status, WNOHANG);

    /*Caso o filho tenha sido terminado por um sinal, mais especificamente pelos sinais SIGUSR1 ou SIGUSR2, o shell envia o mesmo sinal para todos
    os processos criados na mesma sessão, ou seja, que tenham o mesmo group id, encerrando-os*/

    if (WIFSIGNALED(status)){
        if(WTERMSIG(status) == SIGUSR1 || WTERMSIG(status) == SIGUSR2){
            kill(-pid, SIGUSR1);
        }
    }
}

/*Implementação do tratador do sinal SIGUSR do shell*/

void handleSIGUSR(int sig){

    //Cria uma mascara de sinais e inicia
    sigset_t sinais;
    sigemptyset(&sinais);

    /*Adiciona os sinais de interrupcao nessa mascara*/
    sigaddset(&sinais, SIGINT);
    sigaddset(&sinais, SIGQUIT);
    sigaddset(&sinais, SIGTSTP);

    /*Bloqueia todos os sinais adicionados na mascara*/
    sigprocmask(SIG_BLOCK, &sinais, NULL);

    /*Trata o sinal, imprimindo ascii*/

    printf("\n\n");
    printf("            _  _\n");
    printf("  _ _      (0)(0)-._  _.-'^^'^^'^^'^^'^^'--.\n");
    printf(" (.(.)----'`        ^^'                /^   ^^-._\n");
    printf(" (    `                 \\             |    _    ^^-._\n");
    printf("  VvvvvvvVv~~`__,/.._>  /:/:/:/:/:/:/:/\\  (_..,______^^-.\n");
    printf("   `^^^^^^^^`/  /   /  /`^^^^^^^^^>^^>^`>  >        _`)  )\n");
    printf("            (((`   (((`          (((`  (((`        `'--'^\n");
    printf("I feel weird...\n");

    /*Desbloqueia os sinais bloqueados pela mascara*/

    sigprocmask(SIG_UNBLOCK, &sinais, NULL);
}