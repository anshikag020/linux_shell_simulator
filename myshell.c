#include<stdio.h>
#include<stdlib.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdbool.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<ctype.h>
#include "header.h"


char* generate_prompt(char prompt[200]){
    strcpy(prompt,getlogin());
    strcat(prompt,"@");
    char buf[100];
    gethostname(buf,100);
    strcat(prompt,buf);
    strcat(prompt,":~");
    strcat(prompt,getcwd(buf,100));
    strcat(prompt,"$ ");
    return prompt;
}

char* read_word(char word[],char ptr[],int* i){
    while(ptr[*i]==' '||ptr[*i]=='\t') (*i)++;
    int k;
    for(k=0 ; ptr[*i]!=' ' && ptr[*i]!='\t' && ptr[*i]!='\0';k++){
        word[k]=ptr[*i];
        (*i)++;
    }
    word[k]='\0';
    return word;
}

typedef struct command{
    char* command_line;
    struct command* next;
}command;



int main(){
    char prompt[200];
    char word[50];
    command* head=(command*)malloc(sizeof(command));
    command* current_position=head;
    current_position->command_line=readline(generate_prompt(prompt));



    while(1){
        int i=0;
        if(strcmp(read_word(word,current_position->command_line,&i),"\0")==0){       //command is empty
            free(current_position->command_line);
            current_position->command_line=readline(generate_prompt(prompt));
            continue;
        }
        add_history(current_position->command_line);

        if(strcmp(word,"exit")==0){     //exit command
            if(strcmp(read_word(word,current_position->command_line,&i),"\0")==0){
                break;
            }
            else{
                printf("command doesn't exist\n");
                
            }
        }

        else if(strcmp(word,"ls")==0){
            ls(current_position->command_line,i);
        }
        else if(strcmp(word,"ps")==0){
            ps(current_position->command_line,i);
        }
        else if(strcmp(word,"cp")==0){
            cp(current_position->command_line,i);
        }
        else if(strcmp(word,"mv")==0){
            mv(current_position->command_line,i);
        }
        else if(strcmp(word,"grep")==0){
            grep(current_position->command_line,i);
        }
        else if(strcmp(word,"help")==0){
            if(strcmp(read_word(word,current_position->command_line,&i),"\0")){
                printf("invalid command\n");
            }
            else{
                FILE* help=fopen("help2.txt","r");
                char ch;
                while((ch=getc(help))!=EOF){
                    putchar(ch);
                }
                fclose(help);
                printf("\n");
            }
        }
        else{
            printf("invalid command\n");
        }
        current_position->next=(command*)malloc(sizeof(command));
        current_position=current_position->next;
        current_position->command_line=readline(generate_prompt(prompt));
    }

    current_position->next=NULL;

    current_position=head;
    while(current_position->next != NULL){
        free(current_position->command_line);
        head=current_position->next;
        free(current_position);
        current_position=head;
    }

    free(current_position->command_line);
    free(current_position);
    return 0;
}