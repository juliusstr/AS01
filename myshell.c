#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUE 1
#define spaceChar ' '
#define inputMaxSize 10000
#define argPip 20 //max number of pips
#define argArg 20 //max number of arguments//nix pille

void type_prompt();
void readCommand(char* arg[argPip][argArg], char* input);

int main(void)
{
    char input[inputMaxSize];
    char* arg[argPip][argArg];
    int i = 0;
    char * status;

    while(TRUE) {

        type_prompt();
        readCommand(arg, input);
        if (strcmp(arg[0][0], "exit") == 0)
            exit(0);
        if ( (fork())  != 0){
            waitpid(-1,status, 0);
        } else {
            execlp(arg[i][0], arg[i][0], arg[i][1], arg[i][2], arg[i][3], arg[i][4]
                   , arg[i][5], arg[i][6], arg[i][7], arg[i][8], arg[i][9], arg[i][10], arg[i][11]
                   , arg[i][12], arg[i][13], arg[i][14], arg[i][15], arg[i][16], arg[i][17], arg[i][18]
                   , arg[i][19], NULL);
            printf("error\n");
            exit(0);
        }
    } 
    return 0;
}
 
void type_prompt(){;
    char buf[300];
    printf("%s # ", getcwd(buf, sizeof(buf)));
    return;
}

void readCommand (char* arg[argPip][argArg], char *input) {
    int a = 0;
    for (int i = 0; i < inputMaxSize; ++i) {
        input[i] = '\0';
    }

    for (int i = 0; i < argPip; i++) {
        for (int j = 0; j < argArg; j++) {
            arg[i][j] = NULL;
        }
    }
    fgets(input, inputMaxSize, stdin);
    arg[0][0] = input;

    for (int i = 0; i < inputMaxSize; i++) {
        if(input[i] == '\n')
            input[i] = '\0';
        if(input[i] == spaceChar) {
            input[i] = '\0';

            arg[0][++a] = &input[i + 1];
        }
    }

    return;
}
