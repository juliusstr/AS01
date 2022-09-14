#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUE 1
#define spaceChar ' '

void type_prompt();
void readCommand(char* com, char* para);
int indexOf(char temp[500], int i);
void setCommand(char *com, char *temp, int space);

void setParameters(char *para, char temp[500], int space);

int main(int argc, char const *argv[])
{
    char command[50], parameters[500];
    char * status;

    while(TRUE) {
        type_prompt();
        readCommand(command, parameters);
        printf("\ncom: %s\npara: %s\n\n", command, parameters);

        if ( (fork())  != 0){
            waitpid(-1,status, 0);
        } else {
            execve("ls", "", 0);
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

    fgets(temp, sizeof(temp), stdin);
    int indexOfSpace = indexOf(temp, spaceChar);
    setCommand(com, temp, indexOfSpace);
    setParameters(para, temp, indexOfSpace);

    
    return;
}

void setParameters(char *para, char * temp, int space) {
    for (int i = space+1; i < strlen(para); ++i) {
        para[i-(space+1)] = temp[i];
    }
}

void setCommand(char *com, char *temp, int space) {
    for (int i = 0; i < space; ++i) {
        com[i] = temp[i];
    }
}

int indexOf(char *temp, int i) {
    int j = 0;
    while (temp[j] != i){
        j++;
    }
    return j;
}
 
