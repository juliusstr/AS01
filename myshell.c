#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define spaceChar ' ' 
#define inputMaxSize 10000 // Max size of total command including pipes
#define argPip 20 //max number of pipes
#define argArg 20 //max number of arguments

void type_prompt(); // prints cwd
void readCommand(char* arg[argPip][argArg], char* input, int* numberOfPips); // read the command from the useer and splits int in to tokens
int isPipOrSpace(char *input, int *i); //moves index and returns 1 if next input is |, returns 2 if next input is ' ', returns 0 otherwise

void stdCall(char *arg[argPip][argArg]);//standard call, executes one command and its args

int pipCall(char* arg[argPip][argArg], int numberOfPips); //pipe call, executes multiple commands with args
void recPipCall(char* arg[argPip][argArg], int numberOfPips, int i, int *fdOld);//recursive pipe call function
void childPipCall (int *fdOld, int *fdNew, char *arg[argPip][argArg], int i, int numberOfPips);//the command the children execute when theres is a pipeCall

int main(void)
{
    //---------- SETUP ----------
    char input[inputMaxSize];// array for user input
    char* arg[argPip][argArg];// 2d array for pointers to the tokens from user input


    while(true) {
        int numberOfPips = 0;// inits/resets number of pipes
        type_prompt();//prints cwd
        readCommand(arg, input, &numberOfPips);//reads user input and gets tokens to pass to stdCall or pipecall
        if (strcmp(arg[0][0], "exit") == 0)// if the user types exit
            exit(0);
        if (strcmp(arg[0][0], "cd") == 0){// if user types cd to change dir
            chdir(arg[0][1]);//uses the built-in function in c to change dir
            continue;
        }

        if(numberOfPips == 0){ // if there is 0 pipes in the user input a standard call is called
            stdCall(arg); //passes args to stdCall
        } else { 
            pipCall(arg, numberOfPips); // passes args and number of pipes to pipeCall
        }
    }
    return 0;
}

int pipCall(char* arg[argPip][argArg], int numberOfPips){
    //set up for recPipcall;
    int fd[2];
    if(pipe(fd) == -1) // creates a pipe
        return -1;

    recPipCall(arg,numberOfPips,0,fd); // recPipeCall gets the info form pipCall and the pipe created just above
    return 1;
}

void recPipCall(char* arg[argPip][argArg], int numberOfPips, int i, int *fdOld){
    int fdNew[2]; // creates the pipe that will take the output from an exec
    if(pipe(fdNew) == -1) {
        printf("Pipe Error\n\n");
        exit(1);
    }

    int pid1 = fork(); // saves the pid so the difference between parent and child are known
    if (pid1 == -1) { // if fork fails
        printf("error\n");
        exit(1);
    }

    if(pid1 == 0){ // child
        childPipCall(fdOld, fdNew, arg, i, numberOfPips); // the child then runs the exec nad reroutes the inputs and outputs to the appropriate fd's
    }
    close(fdOld[0]); // closes the old fd so there is no loose ends
    close(fdOld[1]);
    waitpid(pid1, NULL,0); // waits for child to finish
    if(numberOfPips > 0)
        recPipCall(arg, --numberOfPips, ++i, fdNew); // the recursive call. The new fd created above is passed as the old fd to create a rotation in fd's
}

void childPipCall (int *fdOld, int *fdNew, char *arg[argPip][argArg], int i, int numberOfPips){
    if(numberOfPips==0) { // if there is no more pipes only the input needs to be mapped
        dup2(fdOld[0], STDIN_FILENO);
    } else if (i == 0){ // if this is the first child to be run. only the output needs to be mapped
        dup2(fdNew[1], STDOUT_FILENO);
    } else { // otherwise both the input and output need to be mapped
        dup2(fdOld[0], STDIN_FILENO);
        dup2(fdNew[1], STDOUT_FILENO);
    }
    close(fdNew[0]); // closes all fd's
    close(fdNew[1]);
    close(fdOld[0]);
    close(fdOld[1]);
    execvp(arg[i][0], arg[i]); // makes the exec call to execute the command with the arguments
    printf("error\n"); // i case exec fails
    exit(1);
}

void stdCall(char *arg[argPip][argArg]) {
    int pid = fork(); // saves the pid so the difference between parent and child are known
    if (pid == -1) {  // if the fork fails
        printf("error\n");
        exit(1);
    }
    if ( pid != 0){ // parent waits for child
        waitpid(pid, NULL,0);
    } else {
        execvp(arg[0][0], arg[0]); // child runs the exec with command at arg[0][0]and the command and arg at arg[0]
        printf("error\n"); // if exec fails
        exit(1);
    }
}
 
void type_prompt(){;
    char buf[300]; // 300 char buffer for the cwd
    printf("\n%s # ", getcwd(buf, sizeof(buf)));// gets the cwd and prints it on a new line
    return;
}

void readCommand (char* arg[argPip][argArg], char *input, int *numberOfPips) {
    int argNumber = 0; //inits the number of args to 0, important for later
    for (int i = 0; i < inputMaxSize; ++i) { // clears the input array
        input[i] = '\0';
    }

    for (int i = 0; i < argPip; i++) { // clears the 2nd char pointer array
        for (int j = 0; j < argArg; j++) {
            arg[i][j] = NULL;
        }
    }

    fgets(input, inputMaxSize, stdin); // gets input from user
    arg[*numberOfPips][argNumber] = input; // sets the first (0,0) in 2d array to the beginning of the input array, since this is where the first command is located.

    for (int i = 0; i < inputMaxSize; i++) {// loops through the input array
        if (input[i] == '\n') {// if the current char in input is \n replaces it with \0 so the exec works.
            input[i] = '\0';
            break;// breaks since there is no more input to analyze
        }
        int pipOrSpace = isPipOrSpace(input, &i);// finds the beginning og the next arg or command if there is a pipe.
        if(pipOrSpace == 2){// if the token is an arg
            arg[*numberOfPips][++argNumber] = &input[i]; // saves a pointer to the arg in the 2d char pointer array
        } else if (pipOrSpace == 1){ // if the next token is a command(AKA there has been a pipe)
            (*numberOfPips)++;
            argNumber = 0; // resets the arg number
            arg[*numberOfPips][argNumber] = &input[i];// saves a pointer to the command in the 2d char pointer array
        }
    }
}

int isPipOrSpace(char *input, int *i){ //return 1 if next input is |, return 2 if next input is ' ', return 0 otherwise
    int returnVal = 0; // setup for the return val.
    while (input[*i] == ' ' || input[*i] == '|') { // continues through the input array until the next char in not ' ' or '|'
        if (input[*i] == '|') { // if the current char is a | the return val is set to 1 and the | is replaced with \0
            input[*i] = '\0';
            returnVal = 1;
        } else if (input[*i] == ' ') { // if the char is a ' '
            if (returnVal != 1) // if the return val is not already set to pipe set it to 2
                returnVal = 2;
            input[*i] = '\0'; // replaces ' ' with \0
        }
        (*i)++; //moves to the next char in input array
    }
    return returnVal;
}

