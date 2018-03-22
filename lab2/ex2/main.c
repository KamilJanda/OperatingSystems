#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <ftw.h>



#define FREE(p)   do { free(p); (p) = NULL; } while(0)

time_t get_time_from_string(char* date)
{
  struct tm time;
  strptime(date, "%Y-%m-%d %H:%M:%S", &time);
  return mktime(&time);
}

int before(time_t a, time_t b) 
{
    return difftime(a, b) > 0.00001 ? 1 : 0;
}

int equal(time_t a, time_t b) 
{
    return fabs(difftime(a, b)) < 0.00001 ? 1 : 0;
}

int after(time_t a, time_t b) 
{
    return difftime(b, a) > 0.00001 ? 1 : 0;
}

char * make_permisions(int mode)
{
    char* result = calloc(11,sizeof(char));

    result[0] = '-';
    result[1] = (mode & S_IRUSR) ? 'r' : '-';
    result[2] = (mode & S_IWUSR) ? 'w' : '-';
    result[3] = (mode & S_IXUSR) ? 'x' : '-';
    result[4] = (mode & S_IRGRP) ? 'r' : '-';
    result[5] = (mode & S_IWGRP) ? 'w' : '-';
    result[6] = (mode & S_IXGRP) ? 'x' : '-';
    result[7] = (mode & S_IROTH) ? 'r' : '-';
    result[8] = (mode & S_IWOTH) ? 'w' : '-';
    result[9] = (mode & S_IXOTH) ? 'x' : '-';
    result[10] = '\0';

    return result;
}

char* make_path(char* path,char* fileName)
{
    char *resultPath = malloc(strlen(path) + strlen(fileName) + sizeof('/'));
    strcpy(resultPath, path);
    strcat(resultPath, "/");
    strcat(resultPath, fileName);

    return resultPath;
}

char* absolute_path(const char* path)
{
    char *result = malloc(4096);
    realpath(path, result);
    return result;
}

void find_files(char* rootName, char* pathFromRoot,time_t date,int (*compare)(time_t, time_t))
{
    DIR* dir = NULL;
    if((dir = opendir(pathFromRoot)) == NULL)
    {
        printf("ERROR: cannot open file \n");
        exit(1);
    }
    struct dirent *currentFile = NULL;
    char* currentFileName = NULL;

    struct stat *buf = malloc(sizeof(struct stat));

    //printf("1. rootName: %s \n",rootName);
    

    while((currentFile = readdir(dir)) != NULL)
    {
        //printf("2. currentFileName: %s \n",currentFile-> d_name);
        if(strcmp(currentFile -> d_name,".") == 0 || strcmp(currentFile -> d_name,"..") == 0) continue;
        
        currentFileName = malloc(strlen(currentFile -> d_name));
        strcpy(currentFileName,currentFile -> d_name);

        char *path = make_path(pathFromRoot, currentFileName);

        lstat(path, buf);
        
        if( S_ISDIR(buf -> st_mode))
        {
            find_files(currentFileName,path,date,compare);
        }

        if( S_ISREG(buf -> st_mode))
        {
            if(compare(date,buf -> st_mtime))
            {
                printf("src :%s, size: %lld bytes, %s, last modification: %s\n",absolute_path(path),(long long) buf->st_size, make_permisions(buf->st_mode),ctime(&(buf->st_mtime)));
            }
        }

        FREE(path);
        FREE(currentFileName);
    }
    

    FREE(buf);
    closedir(dir);
}

//  NFTW   ////////////////////////

int (*COMPARE)(time_t, time_t);
time_t GLOBAL_DATE;

int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
    if((tflag == FTW_F) && COMPARE(GLOBAL_DATE,sb -> st_mtime))
    {
        printf("src :%s, size: %lld bytes, %s, last modification: %s\n",absolute_path(fpath),(long long) sb->st_size, make_permisions(sb->st_mode),ctime(&(sb->st_mtime)));
    }
    return 0;
}

void find_files_nftw(char* path,time_t date,int (*compare)(time_t, time_t))
{
    COMPARE = compare;
    GLOBAL_DATE = date;

    int nopenfd = 100;
    int flags = 0;
    nftw(path, &display_info, nopenfd, flags);
}

void print_help()
{
    printf("Example call: ./main file_name [<,=,>] YYYY-MM-DD HH-MM-SS (optional)nftw");
}


void parser(int argc, char *argv[])
{
    if(argc<5)
    {
        printf("Exception: Too few arguments \n");
        print_help();
        exit(1);
    }

    char* rootName = argv[1];
    char* operator = argv[2];

    char* dateYYmmDD = argv[3];
    char* dateHHMMSS = argv[4];

    char* dateStr = malloc(20);
    strcpy(dateStr,dateYYmmDD);
    strcat(dateStr," ");
    strcat(dateStr,dateHHMMSS);

    time_t date = get_time_from_string(dateStr);

    FREE(dateStr);

    DIR *rootDir = NULL;
    if((rootDir = opendir(rootName)) == NULL)
    {
        printf("ERROR: Invalid dir name in first argumet \n");
        exit(1);
    }
    closedir(rootDir);



    if(strcmp(operator,"<") == 0)
    {
        if(argc == 6 && strcmp(argv[5],"nftw") == 0)
        {
            find_files_nftw(rootName,date,&before);
        }
        else
        {
            find_files(rootName,rootName,date,&before);
        }
    }
    else if(strcmp(operator,"=") == 0)
    {
        if(argc == 6 && strcmp(argv[5],"nftw") == 0)
        {
            find_files_nftw(rootName,date,&equal);
        }
        else
        {
            find_files(rootName,rootName,date,&equal);
        }
    }
    else if(strcmp(operator,">") == 0)
    {
        if(argc == 6 && strcmp(argv[5],"nftw") == 0)
        {
            find_files_nftw(rootName,date,&after);
        }
        else
        {
            find_files(rootName,rootName,date,&after);
        }
    }
    else
    {
        printf("ERROR: Invalid second argument \n");
        exit(1);
    }
    
    
    
}


// MAIN ////////////////////

int main(int argc, char *argv[])
{
    parser(argc, argv);



    return 0;
}

