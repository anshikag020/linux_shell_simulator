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
#include "header.h"


typedef enum e_flags{l,a,g,i,m,n,s,one}e_flags ;

typedef struct file{
    char d_name[100];
    struct stat file_info;
} file;

void bubble_sort(file arr[], int n) {
    int i, j;
    file temp;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (strcmp(arr[j].d_name, arr[j+1].d_name) > 0) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

void print_mode(int mode){
    if(S_ISREG(mode))   printf("-");
    else if (S_ISDIR(mode))     printf("d");
    else if(S_ISCHR(mode))      printf("c");
    else if(S_ISBLK(mode))      printf("b");
    else if(S_ISFIFO(mode))     printf("f");
    else if(S_ISLNK(mode))      printf("l");
    else printf("s");
    if(mode & S_IRUSR) printf("r");
    else printf("-");
    if(mode & S_IWUSR) printf("w");
    else printf("-");
    if(mode & S_IXUSR) printf("x");
    else printf("-");
    if(mode & S_IRGRP) printf("r");
    else printf("-");
    if(mode & S_IWGRP) printf("w");
    else printf("-");
    if(mode & S_IXGRP)  printf("x");
    else printf("-");
    if(mode & S_IROTH)  printf("r");
    else printf("-");
    if(mode & S_IWOTH)  printf("w");
    else printf("-");
    if(mode & S_IXOTH)  printf("x");
    else printf("-");
    printf(" ");
}

void print_owner(int uid){
    struct passwd *pw=getpwuid(uid);
    if(pw==NULL) return;
    else printf("%s ",pw->pw_name);
}

void print_group(int st_gid){
    struct group *grp=getgrgid(st_gid);
    if(grp==NULL) return;
    else printf("%s ",grp->gr_name);
}

void print_date(size_t* mtime){
    struct tm* modified_time = localtime(mtime);
    char modified_date[20];
    strftime(modified_date,sizeof(modified_date),"%b %2d %H:%M",modified_time);
    printf("%s  ",modified_date);
}



void ls(char *ptr, int i ){             // -l, -a, -g, -i, -m, -n, -1
    char word[50];
    int count_files=0;
    DIR* directory=opendir(".");
    if( directory ==NULL){
        printf("cannot access directory\n");
        return;
    }
    struct dirent* entity;
    entity= readdir(directory);
    
    char flag[]={'l','a','g','i','m','n','s','1'};
    int boolean[]={0,0,0,0,0,0,0,0};
    while(strcmp(read_word(word,ptr,&i),"\0")!=0){
        if(word[0]!='-'){
            printf("invalid command\n");
            return;
        }
        for(int counter=1;word[counter]!='\0';counter++){
            char found='0';
            for(int j=0;j<8;j++){
                if(word[counter]==flag[j]){
                    boolean[j]=1;
                    found='1';
                }
            }
            if(found=='0'){
                printf("command doesn't exist\n");
                return;
            }
        }
    }

    while(entity!=NULL){
        count_files++;
        entity=readdir(directory);
    }

    file arr[count_files];
    rewinddir(directory);
    entity=readdir(directory);
    int count=0;
    while(count<count_files){
        strcpy(arr[count].d_name,entity->d_name);
        count++;
        entity=readdir(directory);
    }
    
    bubble_sort(arr,count_files);
    
    if(boolean[m]==1){
        count=0;
        while(count<count_files){
            printf("%s, ",arr[count].d_name);
            count++;
        }
        closedir(directory);
        printf("\n");
        return;
    }

    int sum=0;
    for(int i=0;i<8;i++){
        sum+=boolean[i];
    }    
    if (sum==1 && boolean[one]==1  || sum==2 && boolean[one] && boolean[a]){
        count=0;
        while(count<count_files){
            if(boolean[a] || (arr[count].d_name)[0]!='.'){
                printf("%s\n",arr[count].d_name);
            }
            count++;
        }

        closedir(directory);
        printf("\n");
        return;
    }
    
    
    if(sum==0 || (sum==1 && boolean[a]))      //only ls
    {
        count=0;
        while(count<count_files){
            if((arr[count].d_name)[0] != '.'  ||  boolean[a]){
                printf("%s  ",arr[count].d_name);
                
            }
            count++;
        }
        closedir(directory);
        printf("\n");
        return;
    }


      if(sum==1 && boolean[3]  ||  sum==2 && boolean [a] &&boolean[3]){     //look for i and a
        count=0;       
        while(count<count_files){
            if((arr[count].d_name)[0] != '.'  ||  boolean[a]){
                stat(arr[count].d_name,&arr[count].file_info);
                printf("%ld     ",(arr[count].file_info).st_ino);
                printf("%s\n",arr[count].d_name);
            }
            count++;
        }
        return;
    }

    count=0;
    while(count<count_files){
        if(boolean[a] || (arr[count].d_name)[0] != '.'){
            stat(arr[count].d_name,&arr[count].file_info);
        }
        else {
            count++;
            continue;
        }
        
        if(boolean[3]) printf("%ld ",(arr[count].file_info).st_ino);
        print_mode((arr[count].file_info).st_mode);
        printf("%ld ",(arr[count].file_info).st_nlink);
        if(!boolean[g] && !boolean[n])print_owner((arr[count].file_info).st_uid);
        if(!boolean[n]) print_group((arr[count].file_info).st_gid);
        if(boolean[n]){  
            printf("%d ",(arr[count].file_info).st_uid);
            printf("%d ",(arr[count].file_info).st_gid);    
        }
        printf("%6ld ",(arr[count].file_info).st_size);
        print_date(&((arr[count].file_info).st_mtime));
        printf("%s\n",arr[count].d_name);
        count++;

    }

}
