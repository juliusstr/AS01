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

int pipCall(char* arg[argPip][argArg], int numberOfPips);
void recPipCall(char* arg[argPip][argArg], int numberOfPips, int i, int *fdOld);
void childPipCall (int *fdOld, int *fdNew, char *arg[argPip][argArg], int i, int numberOfPips);

int main(void)
{
    char input[inputMaxSize];
    char* arg[argPip][argArg];
    int temp[2];

    while(true) {
        int numberOfPips = 0;
        type_prompt();
        readCommand(arg, input, &numberOfPips);
        if (strcmp(arg[0][0], "exit") == 0)
            exit(0);
        if (strcmp(srg[0][0], "cd") == 0){
            chdir(arg[0][1]);
            continue;
        }

        if(numberOfPips == 0){
            stdCall(arg);
        } else {
            pipCall(arg, numberOfPips);
        }
    }
    return 0;
}

int pipCall(char* arg[argPip][argArg], int numberOfPips){
    int fd[2];
    if(pipe(fd) == -1)
        return -1;

    recPipCall(arg,numberOfPips,0,fd);
    return 1;
}

void recPipCall(char* arg[argPip][argArg], int numberOfPips, int i, int *fdOld){
    int fdNew[2];
    if(pipe(fdNew) == -1) {
        printf("Pipe Error\n\n");
        exit(1);
    }

    int pid1 = fork();
    if (pid1 == -1) {
        printf("error\n");
        exit(1);
    }

    if(pid1 == 0){ //child
        childPipCall(fdOld, fdNew, arg, i, numberOfPips);
    }
    close(fdOld[0]);
    close(fdOld[1]);
    waitpid(pid1, NULL,0);
    if(numberOfPips > 0)
        recPipCall(arg, --numberOfPips, ++i, fdNew);
}

void childPipCall (int *fdOld, int *fdNew, char *arg[argPip][argArg], int i, int numberOfPips){
    if(numberOfPips==0) {
        dup2(fdOld[0], STDIN_FILENO);
    } else if (i == 0){
        dup2(fdNew[1], STDOUT_FILENO);
    } else {
        dup2(fdOld[0], STDIN_FILENO);
        dup2(fdNew[1], STDOUT_FILENO);
    }
    close(fdNew[0]);
    close(fdNew[1]);
    close(fdOld[0]);
    close(fdOld[1]);
    execvp(arg[i][0], arg[i]);
    printf("error\n");
    exit(1);
}

void stdCall(char *arg[argPip][argArg]) {
    int pid = fork();
    if (pid == -1) {
        printf("error\n");
        exit(1);
    }
    if ( pid != 0){
        waitpid(pid, NULL,0);
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
    for (int i = 0; i < inputMaxSize; ++i) {
        input[i] = '\0';
    }

    for (int i = 0; i < argPip; i++) {
        for (int j = 0; j < argArg; j++) {
            arg[i][j] = NULL;
        }
    }

    fgets(input, inputMaxSize, stdin);
    arg[*numberOfPips][argNumber] = input;

    for (int i = 0; i < inputMaxSize; i++) {
        if (input[i] == '\n'){
            input[i] = '\0';
        }else if(input[i] == spaceChar) {
            input[i] = '\0';
            arg[*numberOfPips][++argNumber] = &input[i + 1];
        } else if(input[i] == '|'){
            input[i] = '\0';
            while (input[i] == spaceChar){
                input[i] = '\0';
                i++;
            }
            (*numberOfPips)++;
            argNumber = 0;
            arg[*numberOfPips][argNumber] = &input[i+1];
        }
    }
}
