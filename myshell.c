#include<stdio.h>
#include<stdlib.h>

#define TRUE 1

void type_prompt();
void readCommand(char* com, char* para);
void waitpid(int a , char* sta, int b);

int main(int argc, char const *argv[])
{
    char command[50], parameters[500];
    char * status;
    while(TRUE) {
        type_prompt();
        readCommand(command, parameters);

        if (fork()  != 0){
            waitpid(-1,status, 0);
        } else {
            execve(command, parameters, 0);
            exit(1);
        }
    } 
    return 0;
}
 
void type_prompt(){
    char * text = {"Type command: "};
    printf("%s", text);
    return;
}

void readCommand (char* com, char* para) {
    char temp[500];
    int spaceChar = (int) ' ';
    scanf("%499s", temp);
    printf("%s\n", temp);
    
    return;
}

void waitpid(int a , char* sta, int b) {
    return;
}
 
