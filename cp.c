#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include"header.h"


typedef enum cp_flags{r,I,b,f} cp_flags;



void free_everything(entity_list* head){
    entity_list* curr_position=head;
    while(strcmp(curr_position->entity,"\0")){
        curr_position=head->next;
        free(head);
        head=curr_position;
    }
    free(head);
} 

int isDir(const char* fileName)
{
    struct stat path;

    stat(fileName, &path);

    return !(S_ISDIR(path.st_mode));       //return 0 if it is directory
}


void cp(char ptr[],int i){
    char word[50];
    bool found[4]={false,false,false,false};
    entity_list* head= (entity_list*)malloc(sizeof(entity_list));
    entity_list * curr_position=head;
    strcpy(curr_position->entity,"\0");

    //checking for flags
    while(strcmp(read_word(word,ptr,&i),"\0")){         
        if(word[0]=='-' && word[1]!='-'){
            int counter=1;
            while(word[counter]!='\0'){
                if(word[counter]=='r' || word[counter]=='R') found[r]=true;
                else if(word[counter]=='i') found[I]=true;
                else if(word[counter]=='b')found[b]=true;
                else if(word[counter]=='f') found[f]=true;
                else {
                    printf("-%c is invalid flag\n",word[counter]);
                    free(head);
                    free(curr_position);
                    return;
                }
                counter++;
            }
        }
        else if(word[0]=='-' && word[1]=='-' && word[2]=='\0') {
            //found_entity=true;
            break;
        }
        else if(word[0]=='-' && word[1]=='-')   printf("invalid command\n");
        else{
            //found_entity=true;
            break;
        }    
    }


    //saving the entities in a linked list
    int quote_count=0;
    while(strcmp(word,"\0")){
        if(strcmp(word,"--")==0){
            read_word(word,ptr,&i);
            continue;
        }
        strcat(curr_position->entity,word);
        
        for(int j=0;j<strlen(word);j++){
            if(word[j]=='\''){
                quote_count++;
            }
        }
        if( quote_count%2 || word[strlen(word)-1]=='\\'){
            strcat(curr_position->entity," ");
        }
        else{
            curr_position->next=(entity_list*)malloc(sizeof(entity_list));
            curr_position=curr_position->next;
            strcpy(curr_position->entity,"\0");
            quote_count=0;
        }
        read_word(word,ptr,&i);
    }
    

    //for removing indentation such as / and '
    curr_position= head;
    while(strcmp(curr_position->entity,"\0")){
        int j=0,k=0;
        while((curr_position->entity)[j]!='\0'){
            if((curr_position->entity)[j]!='\\' && (curr_position->entity)[j]!='\'')
                (curr_position->entity2)[k++]=(curr_position->entity)[j++];
            else j++;
        }
        (curr_position->entity2)[k]='\0';
        curr_position=curr_position->next;
    }



    



    entity_list* tail=head;
    curr_position=head;    
    //checking if the entities exist and finding the tail
    while(strcmp(curr_position->entity,"\0")){
        tail=curr_position;
        curr_position=curr_position->next;
    }
    curr_position=head;
    while(curr_position!=tail){
        if(access(curr_position->entity2,F_OK)){
            printf("%s is not a file or directory\n",curr_position->entity);
            curr_position=head;
            free_everything(head);
            return;
        }
        curr_position=curr_position->next;
    }


    if(head==tail){     //user entered only one entity
        printf("missing destination file\n");
    }

    //finding the source file/directory names
    curr_position=head;
    while(curr_position!=tail){
        int j=strlen(curr_position->entity2)-1;
        while((curr_position->entity2)[j]!='/' && j>=0){
            j--;
        }
        j++;
        strcpy(curr_position->file,&(curr_position->entity2)[j]);
        curr_position=curr_position->next;
    }


    //if there are only two entries and they are files
    curr_position=head;
    if(isDir(head->entity2) && isDir(tail->entity2) && curr_position->next ==tail){
        if(found[I] && access(tail->entity2,F_OK)==0){
            printf("overwrite %s file?: ",tail->entity);
            char ch[5];
            scanf("%s",ch);
            if(strcmp(ch,"y")){
                free_everything(head);
                return;
            }
        }
        if(found[b] && access(tail->entity,F_OK)==0){
            char backup[100];
            strcpy(backup,tail->entity2);
            strcat(backup,"~");
            FILE* dest_file=fopen(tail->entity2,"r");
            FILE* backup_file=fopen(backup,"w");
            char ch;
            while((ch=getc(dest_file))!=EOF){
                putc(ch,backup_file);
            }
            fclose(dest_file);
            fclose(backup_file);
        }
        FILE* source_file=fopen(head->entity2,"r");
        FILE* dest_file=fopen(tail->entity2,"w");
        if(source_file ==NULL){
            printf("cannot open %s file: permission denied\n",head->entity);
            return;
        }
        else if(dest_file==NULL){
            if(!found[f]){
                printf("cannot create regular file %s: Permission denied\n",tail->entity);
                return;
            }
            else{
                remove(tail->entity2);
                dest_file=fopen(tail->entity2,"w");
            }
        }
        char ch;
        while((ch=getc(source_file))!=EOF){
            putc(ch,dest_file);
        }
        fclose(source_file);
        fclose(dest_file);
        free_everything(head);
        return;
    }


    else if(isDir(tail->entity2)){
        printf("%s is not a directory\n",tail->entity);      //free over here
        free_everything(head);
        return;
    }
    

    //if tail is directory then:
    while(curr_position!=tail){
        //if source is file
        if(isDir(curr_position->entity2)){
            FILE* source_file=fopen(curr_position->entity2,"r");
            char dest_path[100];
            char ch;
            strcpy(dest_path,tail->entity2);
            strcat(dest_path,"/");
            strcat(dest_path,curr_position->file);
            if(found[I] && access(dest_path,F_OK)==0){
                printf("overwrite %s file?: ",dest_path);
                char ch[5];
                scanf("%s",ch);
                if(strcmp(ch,"y")){
                    curr_position=curr_position->next;
                    continue;
                }
            }
            if(found[b] && access(dest_path,F_OK)==0){
                char backup[100];
                strcpy(backup,dest_path);
                strcat(backup,"~");
                FILE* dest_file= fopen(dest_path,"r");
                FILE* backup_file= fopen(backup,"w");
                char ch;
                while((ch=getc(dest_file))!=EOF){
                    putc(ch,backup_file);
                }
                fclose(backup_file);
                fclose(dest_file);
            }
            if(source_file==NULL){
                printf("cannot open %s file: Permission denied",curr_position->entity);
                curr_position=curr_position->next;
                continue;
            }
            FILE* dest_file= fopen(dest_path,"w");
            if(dest_file==NULL){
                if(!found[f]){
                    printf("cannot create regular file %s: Permission denied\n",dest_path);
                    curr_position=curr_position->next;
                    continue;
                }
                else{
                    remove(dest_path);
                    dest_file=fopen(dest_path,"w");
                }
            }
            while((ch=getc(source_file))!=EOF){
                putc(ch,dest_file);
            }
            fclose(dest_file);
            fclose(source_file);
        }
        

        // if source is directory
        else{
            if(!found[r]){
                printf("cp: -r not specified\n");
                return;
            }
            char dest_path[100];
            strcpy(dest_path,tail->entity2);
            strcat(dest_path,"/");
            strcat(dest_path,curr_position->file);
            if(access(dest_path,F_OK))
                if(mkdir(dest_path,0777)){       //couldn't make directory
                    printf("couldn't make copy of %s directory\n",curr_position->entity);
                    curr_position=curr_position->next;
                    continue;
                }
            char dest_file_path[100];
            char source_file_path[100];

            DIR* directory=opendir(curr_position->entity2);
            if(directory==NULL) {printf("cannot access %s directory\n",curr_position->entity);  curr_position= curr_position->next; continue;}
            struct dirent* source_file= readdir(directory);
            while(source_file!=NULL){
                if((source_file->d_name)[0]=='.'){
                    source_file=readdir(directory);
                    continue;
                }
                strcpy(dest_file_path,dest_path);
                strcat(dest_file_path,"/");
                strcat(dest_file_path,source_file->d_name);
                if(found[I] && access(dest_file_path,F_OK)==0){
                    printf("overwrite %s file",dest_file_path);
                    char ch[5];
                    scanf("%s",ch);
                    if(strcmp(ch,"y")){
                        source_file=readdir(directory);
                        continue;
                    }
                }
                strcpy(source_file_path,curr_position->entity2);
                strcat(source_file_path,"/");
                strcat(source_file_path,source_file->d_name);
                FILE* ptr_to_source=fopen(source_file_path,"r");
                FILE* ptr_to_dest=fopen(dest_file_path,"w");
                char ch;
                while((ch=getc(ptr_to_source))!=EOF){
                    putc(ch,ptr_to_dest);
                }
                fclose(ptr_to_source);
                fclose(ptr_to_dest);
                source_file=readdir(directory);
            } 
            closedir(directory);
        }
        curr_position=curr_position->next;
    }
    free_everything(head);
}