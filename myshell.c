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
    /*temp[0] = 0;
    temp[1] = 0;
    int c;
    int *status;*/



    while(true) {
        int numberOfPips = 0;
        type_prompt();
        readCommand(arg, input, &numberOfPips);
        if (strcmp(arg[0][0], "exit") == 0)
            exit(0);

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
        printf("eror");
        exit(1);
    }

    if(pid1 == 0){ //child
        childPipCall(fdOld, fdNew, arg, i, numberOfPips);
    }
    close(fdOld[0]);
    close(fdOld[1]);
    waitpid(pid1/*pid1*/, NULL,0);
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
    execlp(arg[i][0], arg[i][0], arg[i][1], arg[i][2], arg[i][3], arg[i][4]
            , arg[i][5], arg[i][6], arg[i][7], arg[i][8], arg[i][9], arg[i][10], arg[i][11]
            , arg[i][12], arg[i][13], arg[i][14], arg[i][15], arg[i][16], arg[i][17], arg[i][18]
            , arg[i][19], NULL);
    printf("error\n");
    exit(1);
}

void stdCall(char *arg[argPip][argArg]) {
    int * status;
    int pid = fork();
    if (pid == -1) {
        printf("eror");
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
    int pipNumber = 0;//skift til number of pips
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
            arg[pipNumber][argNumber] = &input[i+1];
        }
    }

    return;
}
