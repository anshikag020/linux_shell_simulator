#define _GNU_SOURCE
#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include"header.h"


typedef enum grep_flag {I,n,c} grep_flag;


void grep(char ptr[],int i){
    char string[200];
    char word[50];
    char file[100];
    strcpy(string,"\0");
    bool found[3]={false,false,false};
    read_word(word,ptr,&i);

    //checking the flags
    while(word[0]=='-'){
        for(int j=1;j<strlen(word);j++){
            if(word[j]=='i') found[I]=true;
            else if(word[j]=='n') found[n]=true;
            else if(word[j]=='c') found[c]=true;
            else {
                printf("-%c is invalid flag\n",word[j]);
                return;
            }
        }
        read_word(word,ptr,&i);
    }


    if(word[0]!='\"'){
        printf("invalid command\n");
        return;
    }

    //finding string
    strcat(string,&word[1]);
    int len=strlen(string);
    i--;
    while(ptr[i]!='\"'){
        if(ptr[i]=='\0'){
            printf("invalid command\n");
            return;
            
        }
        string[len++]=ptr[i++];
    }
    i++;
    string[--len]='\0';


    //taking file name
    while(ptr[i]==' ' || ptr[i] == '\t') i++;
    strcpy(file,&ptr[i]);

    int j;
    for(j=strlen(file)-1;file[j]==' ' || file[j] == '\t';j--);
    j++; file[j]='\0';

    if(access(file,F_OK)){
        printf("%s file doesn't exist\n",file);
        return;
    }


    int line_count=0,occurrences_in_line=0,no_of_occurrences=0;
    FILE* fptr=fopen(file,"r");
    if(fptr==NULL){
        printf("cannot access file\n");
    }
    char line[200];
    char* string_ptr;
    char* curr_ptr;


    if(!found[I]){
        while(fgets(line,200,fptr)!=NULL){
            line_count++;
            occurrences_in_line=0;
            curr_ptr=line;
            while((string_ptr=strstr(curr_ptr,string))!=NULL){
                no_of_occurrences++;
                occurrences_in_line++;
                if(found[n] && occurrences_in_line==1){
                    if(!found[c])printf("%d: ",line_count);
                }
                while(curr_ptr!=string_ptr){
                    if(!found[c])putchar(*curr_ptr);
                    curr_ptr++;
                }
                printf("\033[0;31m");
                int len=strlen(string);
                for(int j=0;j<len;j++){
                    if(!found[c])putchar(*curr_ptr);
                    curr_ptr++;
                }
                printf("\033[0m");
            }
            if(occurrences_in_line && !found[c]){
                printf("%s",curr_ptr);
            }

        }
        if(found[c]) printf("%d\n",no_of_occurrences);
    }


    else{
       while(fgets(line,200,fptr)!=NULL){
            line_count++;
            occurrences_in_line=0;
            curr_ptr=line;
            while((string_ptr=strcasestr(curr_ptr,string))!=NULL){
                no_of_occurrences++;
                occurrences_in_line++;
                if(found[n] && occurrences_in_line==1){
                    if(!found[c])printf("%d: ",line_count);
                }
                while(curr_ptr!=string_ptr){
                    if(!found[c])putchar(*curr_ptr);
                    curr_ptr++;
                }
                printf("\033[0;31m");
                int len=strlen(string);
                for(int j=0;j<len;j++){
                    if(!found[c])putchar(*curr_ptr);
                    curr_ptr++;
                }
                printf("\033[0m");
            }
            if(occurrences_in_line && !found[c]){
                printf("%s",curr_ptr);
            }

        }
        if(found[c]) printf("%d\n",no_of_occurrences);
    }
    
    
    fclose(fptr);    
}
