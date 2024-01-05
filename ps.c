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
#include<limits.h>
#include<fcntl.h>
#include "header.h"




int if_positive_number(char ptr[]){
    int len=strlen(ptr);
    for(int i=0;i<len;i++){
        if(!isdigit(ptr[i])){
            return 0;
        }
    }
    return 1;
}

void print_time(int time){
    int seconds=(float)time/100;
    int hours=seconds/3600;
    int minutes= (seconds/60)%60;
    int secs=seconds %60;
    printf("%02d:%02d:%02d ",hours,minutes,secs);
}

void hrs_min(int time){
    int seconds=(float)time/100;
    int hours=seconds/3600;
    int minutes=(seconds/60)%60;
    printf("%02d:%02d ",hours,minutes);
}


char* ps_tty(char tty[],char processid[]){
    char file_path[100];
    strcpy(file_path,"/proc/");
    strcat(file_path,processid);
    strcat(file_path,"/fd/0");
    
    int fd= open(file_path,O_RDONLY,0666);
    if(fd==-1){
        strcpy(tty,"?");
        return tty;
    }
    char *tty_name=ttyname(fd);
    if(tty_name==NULL){
        strcpy(tty,"?");
        return tty;
    }
    strcpy(tty,tty_name);
    return tty;

}

//checking the flags
typedef enum ps_flags{e,f,l} ps_flags;
void ps(char* ptr, int i){
    char word[50];
    bool found[]={false,false,false};
    
    
    while(strcmp(read_word(word,ptr,&i),"\0")){
        if(word[0]!='-'){
            printf("invalid command\n");
            return;
        }
        int counter=1;
        while(word[counter]!='\0'){
            if(word[counter]=='e') found[e]=true;
            else if(word[counter]=='f') found[f]=true;
            else if(word[counter]=='l') found[l]=true;
            else{
                printf("-%c flag not found\n",word[counter]);
                return;
            }
            counter++;
        }
    }
       
    //printing the headers
    if(found[f] && found[l]){
        printf("S UID      PID  PPID  C PRI NI   SZ WCHAN    STIME  TTY\t\t     TIME    CMD\n");
    }
    else if(found[f]){
        printf("UID\t  PID  PPID C STIME  TTY\t\tTIME   CMD\n");
    }

    else if(found[l]){
        printf("S  UID  PID PPID C PRI NI   SZ  WCHAN \t  TTY\t\t   TIME   CMD\n");
    }
    else{
        printf("PID    TTY\t\tTIME    CMD\n");
    }

    //opening directory
    DIR* directory;
    struct dirent* process;
    directory= opendir("/proc");
    if(directory==NULL){
        perror("opendir");
        return;
    }


    char cmd[1000],state[5],tty[50],wchan_wchan[50];
    int time,tty_nr;
    int stime,utime,ppid,session_id,wchan,start_time,rss;
    unsigned long c_field;
    long priority,nice;
    char line[256];
    char discard[256];
    int discards;
    long l_discards;
    struct passwd* pw;
    char* tty_name=ttyname(STDIN_FILENO);
    char tty__name[50];
    strcpy(tty__name,tty_name);

    //reading all the details for each process
    while((process=readdir(directory))!=NULL){
        
        int uid= INT_MIN;
        if(!if_positive_number(process->d_name)) continue;      //not a process
        char file_path[100];
        strcpy(file_path,"/proc/");         //opening stat file
        strcat(file_path,process->d_name);
        strcat(file_path,"/stat");
        FILE *fptr=fopen(file_path,"r");
        if(fptr==NULL){
            printf("cannot access info\n");
            continue;
        }
        fscanf(fptr,"%d%s%s%d%d%d%d%d%d%d%d%d%d%d%d%ld%ld%ld%ld%ld%lu%d%d%d%ld%d%d%d%d%d%d%d%ld%d%d",&discards,discard,state,&ppid,&discards,&session_id,&tty_nr,&discards,&discards,&discards,&discards,&discards,&discards,&utime,&stime,&l_discards,&l_discards,&priority,&nice,&l_discards,&c_field,&start_time,&discards,&rss,&l_discards,&discards,&discards,&discards,&discards,&discards,&discards,&discards,&l_discards,&discards,&wchan);
        fclose(fptr);


        strcpy(file_path,"/proc/");     //opening status file
        strcat(file_path,process->d_name);
        strcat(file_path,"/status");
        fptr=fopen(file_path,"r");
        if(fptr==NULL){
            printf("cannot access info.\n");
            continue;
        }
        else {
            while(fgets(line,sizeof(line),fptr)){
                if(strncmp(line,"Name:",5)==0){
                    fscanf(fptr,"%*s%s",cmd);
                }
                else if(strncmp(line,"Uid:",4)==0){
                    fscanf(fptr,"%*s%d",&uid);
                }
            }  
        }
        //printing details
        {
            if(ppid==0)continue;
            if(!found[e] && strcmp(tty__name,ps_tty(tty,process->d_name))) continue;
            if(found[l]) printf("%s ",state);
            if(found[l] && !found[f]) printf("%d ",uid);
            else if(found[f]){
                pw=getpwuid(uid);
                if(pw!=NULL){ 
                    printf("%s ",pw->pw_name);
                    int len=9-strlen(pw->pw_name);
                    if(len>0){
                        while(len>0){
                            printf(" ");
                            len--;
                        }
                    }
                }
                else printf(" -        ");
            }
            printf("%s ",process->d_name);
            if(found[l] || found[f]) printf("%d ",ppid);
            if(found[l] || found[f]) printf("%ld ",c_field);
            if(found[l])printf(" %ld ",priority);
            if(found[l]) printf(" %ld ",nice);
            if(found[l]) printf("%d ",rss);
            if(found[l]) {      //wchan
                strcpy(file_path,"/proc/");
                strcat(file_path,process->d_name);
                strcat(file_path,"/wchan");
                fptr=fopen(file_path,"r");
                if(fptr==NULL){
                    printf("-      ");
                }
                else{
                    fgets(wchan_wchan,50,fptr);
                    printf("%s ",wchan_wchan);
                    if(strlen(wchan_wchan)<9){
                        int j=9-strlen(wchan_wchan);
                        while(j>0){
                            printf(" ");
                            j--;
                        }
                    }
                }
            }
            if(found[f]){       //stime
                hrs_min(start_time);
            }
            printf("%s ",ps_tty(tty,process->d_name));
            int len=10-strlen(process->d_name);
            while(len>0){
                printf(" ");
                len--;
            }
            print_time(stime+utime);
            strcpy(file_path,"/proc/");
            strcat(file_path,process->d_name);
            strcat(file_path,"/cmdline");
            FILE* cmdline=fopen(file_path,"r");
            if(cmdline==NULL){
                printf("-\n");
                continue;
            }
            char* cmd=NULL;
            size_t clen=0;
            ssize_t nread=getline(&cmd,&clen,cmdline);
            if(nread==-1){
                printf("-\n");
                continue;
            }
            char* cmd_name = strrchr(cmd, '/') + 1;
            if(cmd_name!=NULL)
                printf("%s\n",cmd_name);
            else printf("\t-\n");
            free(cmd);
            fclose(cmdline);
        }
    }
}
