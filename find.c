/* Dear Professor/TA, below is the usage of my utility.
 * -w location : if location not provided then current directory is used
 * -n = name
 * -i = inode
 * -m = minutes , - means less than and + means greater than
 * -d = delete
 * -e = execute (cat,rm, mv)
 *
 * You can find examples in automate.sh file
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

void find(char* sub_dir, int n, char* toFind, int m, int mmin, int i, int iNode, int d,char* newName, int o,int ren);

void main(int argc, char *argv[]){

        //DECLARING VARS
        int w=0, n=0, m=0, i=0, a=0, mnum=0, iNode=0, d=0,o=0,ren=0;
        char  *where, *name, *mmin, *inum, *action, *ptr, *cmd, *newName;
        char *mv="mv";
        char *del="rm";
        char* cat="cat";
        while(1){
                //GET CHARACTERS ONE BY ONE
                char c;
                c = getopt(argc,argv,"w:n:m:i:a:d:e:");
                if(c==-1){
                        break;
                }
                //IF A CHARCATER IS PROVIDED, SET IT'S VALUE TO ONE
                switch(c){
                        case 'w':
                                w = 1;
                                where = optarg;
                                printf("Where: %s\n", optarg);
                                break;
                        case 'n':
                                n = 1;
                                name = optarg;
                                printf("name: %s\n", optarg);
                                break;
                        case 'm':
                                m = 1;
                                mmin = optarg;
                                printf("mmin: %s\n", optarg);
                                mnum = atoi(mmin);
                                break;
                        case 'i':
                                i = 1;
                                inum = optarg;
                                printf("inum:%s\n",optarg);
                                iNode = atoi(inum);
                                break;
                        case 'd':
                                d = 1;
                                printf("delete mode \n");
                                break;
                        case 'e':
                                cmd = optarg;
                                printf("command: %s\n", optarg);
                                if(strcmp(cmd,mv) == 0){
                                        ren = 1;
                                        newName = argv[optind];
                                        printf("new name: %s\n", newName);
                                }
                                if(strcmp(cmd,del) == 0){
                                        d = 1;
                                }
                                if(strcmp(cmd,cat) == 0){
                                        o=1;
                                }
                                break;
                        case '?':
                        default:
                                printf("An invalid option detected.\n");
                }
        }
        //IF DIRECTORY NOT SPECIFIED THEN USE CURRENT
        if(w!=1)
        {
                where=malloc(sizeof(char)*2000);
                ptr=getcwd(where,2000);
        }
        if(n!=1)
        {
                name=NULL;
        }

        find(where, n, name, m, mnum, i, iNode, d, newName, o, ren);

}

void find(char* sub_dir, int n, char* toFind, int m, int timeMod, int i, int iNode, int d,char* newName,int o,int ren){

        int matchN=0,matchM=0,matchI=0;
        DIR *sub_dp=opendir(sub_dir);
        struct dirent *sub_dirp;
        int confirmBuf;
        time_t currTime;
        struct stat buf;
        char *temp1=".";
        char *temp2="..";
        char *temp3="/";
        if(sub_dp!=NULL){
                while((sub_dirp=readdir(sub_dp))!=NULL){
                        matchN=0;
                        matchM=0;
                        matchI=0;
                        char* temp=sub_dirp->d_name;
                        if(strcmp(temp,temp1)!=0 && strcmp(temp,temp2)!=0){
                                char* temp_sub=malloc(sizeof(char)*2000);
                                strcpy(temp_sub,temp3);
                                strcat(temp_sub,temp);
                                char* temp_full_path=malloc(sizeof(char)*2000);
                                strcpy(temp_full_path,sub_dir);
                                strcat(temp_full_path,temp_sub);
                                confirmBuf= stat(temp_full_path,&buf);
                                if( confirmBuf==0){
                                        //SEARCH BY NAME
                                        if( n==1 && strcmp(sub_dirp->d_name,toFind)==0){
                                                matchN=1;
                                        }
                                        //SEARCH BY MOD TIME
                                        if ( m ==1){
                                                currTime= time(NULL);
                                                //SEARCHING BY NEG TIME
                                                if( timeMod <= 0){
                                                        if( (difftime(currTime, buf.st_mtime)) /60 < abs(timeMod) ){
                                                                matchM=1;
                                                        }
                                                }
                                                //SEARCHING BY POSITIVE TIME
                                                else if(timeMod > 0){
                                                        if ( (difftime(currTime, buf.st_mtime)) /60 > timeMod ){
                                                                matchM=1;
                                                        }
                                                }
                                        }
                                        //SEARCHING BY INODE
                                        if ( i==1 && (int)buf.st_ino == iNode){
                                                matchI=1;
                                        }

                                        //IF EVERYTHING IS SATISFIED
                                        if(n==matchN && m==matchM && i==matchI){

                                                //IF SEARCHING AND DELETING, IF NOT FOLDER THAN DELETE
                                                if( d==1 && S_ISREG(buf.st_mode)){
                                                        printf("DELETING ");
                                                        remove(temp_full_path);
                                                }

                                                //IF CAT COMMAND GIVEN
                                                if(o==1 && S_ISREG(buf.st_mode)){
                                                        FILE *fp;
                                                        char buff[2000];
                                                        char* cmd = malloc(sizeof(char)*2000);
                                                        strcpy(cmd,"cat ");
                                                        strcat(cmd,temp_full_path);
                                                        fp = popen(cmd,"r");
                                                        if(fp==NULL)
                                                                printf("Unable to open file\n");
                                                        else{
                                                                printf("Opening: %s\n",temp_full_path );
                                                                while(fgets(buff,2000,fp)!=NULL){
                                                                        printf("%s",buff);
                                                                }
                                                                pclose(fp);
                                                        }
                                                }
                                                //TRYING TO RENAME
                                                else if(newName!=NULL && ren==1){
                                                        char* temp_sub=malloc(sizeof(char)*2000);
                                                        strcpy(temp_sub,temp3);
                                                        strcat(temp_sub,newName);
                                                        char* temp_new_path=malloc(sizeof(char)*2000);
                                                        strcpy(temp_new_path,sub_dir);
                                                        strcat(temp_new_path,temp_sub);
                                                        if(rename(temp_full_path,temp_new_path)==0 && S_ISREG(buf.st_mode))

                                                                printf("File renamed \n");
                                                        else
                                                                printf("Error renaming the file\n");
                                                }
                                                else
                                                        printf("%s\n",temp_full_path );
                                        }
                                }
                                DIR* subsubdp=opendir(temp_full_path);
                                if(subsubdp!=NULL){
                                        closedir(subsubdp);
                                        find(temp_full_path,n, toFind,m,timeMod, i,iNode, d,newName,o,ren);
                                }
                        }
                }
                closedir(sub_dp);
        }
        else
        {
                printf("Cannot open directory \n");
                exit(2);
        }
}
