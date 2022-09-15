#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define spaceChar ' '
#define inputMaxSize 10000
#define argPip 20 //max number of pips
#define argArg 20 //max number of arguments

void type_prompt();
void readCommand(char* arg[argPip][argArg], char* input, int* numberOfPips);

void stdCall(char *arg[argPip][argArg]);

void pipeCall(char *arg[argPip][argArg], int numberOfPips, bool firstCall, int* pFd, int i);

int main(void)
{
    char input[inputMaxSize];
    char* arg[argPip][argArg];
    int temp[2];
    temp[0] = 0;
    temp[1] = 0;
    int c;
    int *status;



    while(true) {
        int numberOfPips = 0;
        type_prompt();
        readCommand(arg, input, &numberOfPips);
        if (strcmp(arg[0][0], "exit") == 0)
            exit(0);

        printf("numberof pips: %d\n", numberOfPips);
        if(numberOfPips == 0){
            stdCall(arg);
        } else {
            pipeCall(arg, numberOfPips, true, temp, 0);
            printf("\njeg er ude\n");
            waitpid(-1,status, 0);
            //while ((c = getchar()) != '\n' && c != EOF) { }
        }
    }
    return 0;
}

void pipeCall(char *arg[argPip][argArg], int numberOfPips, bool firstCall, int* pFd, int i) {
    //printf("her");
    int *status;
    int fd [2];
    if(pipe(fd) < 0)
        exit(1);

    int pid = fork();
    if(pid == -1)
        exit(1);//err msg?
    if ( pid  != 0){
        close(pFd[0]);
        close(pFd[1]);
        if(numberOfPips != 0){
            numberOfPips = numberOfPips - 1;
            pipeCall(arg, numberOfPips,false, fd, (i++));
        }
        printf("venter på pid\n");
    } else {
        if(!firstCall){
            dup2(pFd[1], STDOUT_FILENO);

        }
        close(pFd[1]);
        close(pFd[0]);

        if(numberOfPips>0){
            dup2(fd[0], STDIN_FILENO);

        }
        close(fd[1]);
        close(fd[0]);

        execvp(arg[numberOfPips][0], arg[numberOfPips]);
        printf("error\n");
        exit(1);
    }


};

void stdCall(char *arg[argPip][argArg]) {
    int * status;
    if ( (fork())  != 0){
        waitpid(-1,status, 0);
    } else {
        execvp(arg[0][0], arg[0]);
        printf("error\n");
        exit(1);
    }
}
 
void type_prompt(){;
    char buf[300];
    printf("%s # ", getcwd(buf, sizeof(buf)));
    return;
}

void readCommand (char* arg[argPip][argArg], char *input, int *numberOfPips) {
    int argNumber = 0;
    int pipNumber = 0;
    for (int i = 0; i < inputMaxSize; ++i) {
        input[i] = '\0';
    }

    for (int i = 0; i < argPip; i++) {
        for (int j = 0; j < argArg; j++) {
            arg[i][j] = NULL;
        }
    }
    fgets(input, inputMaxSize, stdin);
    arg[pipNumber][argNumber] = input;

    for (int i = 0; i < inputMaxSize; i++) {
        if (input[i] == '\n'){
            input[i] = '\0';
        }else if(input[i] == spaceChar) {
            input[i] = '\0';
            arg[pipNumber][++argNumber] = &input[i + 1];
        } else if(input[i] == '|'){
            input[i] = '\0';
            pipNumber++;
            while (input[i] == spaceChar){
                input[i] = '\0';
                i++;
            }
            *numberOfPips = *numberOfPips + 1;//*numberOfPips++;//virker ik :(
            argNumber = 0;
            arg[pipNumber][argNumber] = input;
        }
    }

    return;
}
