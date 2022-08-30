#include<stdio.h>
#include<iostream>
#include<dirent.h>
#include<string.h>
#include <fstream>
#include <experimental/filesystem>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<filesystem>
#include <pwd.h>
#include <grp.h>
#include<stdlib.h>
#include<termios.h>
#include<bits/stdc++.h>
#include<sys/ioctl.h>
#include<errno.h>
using namespace std;

stack<string>s1;
//stack<string>s2;
stack<string>s3;
int m=0;
string kl;
int pointerIndex = 0;
 struct termios orig_termios;

vector<vector<string>>v;

char* user(const char *file)
{
    struct stat info;
    stat(file, &info);  
    struct passwd *pw = getpwuid(info.st_uid);
    return  pw->pw_name;
}

char* group(const char *file)
{
    struct stat info;
    stat(file, &info); 
    struct group  *gr = getgrgid(info.st_gid);
    return gr->gr_name;
}

char* permissions(const char *file){
    struct stat st;
    char *modeval = (char *)malloc(sizeof(char) * 9 + 1);
    if(stat(file, &st) == 0){
        mode_t perm = st.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';
        return modeval;     
    }
    else{
        return strerror(errno);
    }   
}


off_t fsize(string path) {
    struct stat st; 

    if (stat(path.c_str(), &st) == 0)
        return (st.st_size/1024);

    return -1; 
}

string getFileCreationTime(string path) {
    struct stat attr;
    stat(path.c_str(), &attr);
    //printf(" %s", ctime(&attr.st_mtime));
    //cout<<ctime(&attchar* permissions(char *file){
    
    return ctime(&attr.st_mtime);
}

void disableRawMode()
{
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&orig_termios);
}
void enableRawMode()
{
    tcgetattr(STDIN_FILENO,&orig_termios);
    atexit(disableRawMode);

    struct termios raw=orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void RefreshScreen()
{
    cout << "\033[H\033[2J\033[3J" ;
    
}

int getWindowSize(int *rows,int *cols)
{
    struct winsize ws;
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws) == -1 || ws.ws_col==0)
    {
        return -1;
    }
    else{
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void openfile(string dirname)
{
    DIR* dir=opendir(dirname.c_str());
    off_t s;
    char actualpath[PATH_MAX];
    string a,b,c,dl,pp,buffer,fileName;
    char* bo;
    char* co;
    char* d;
    char* ptr;
    char* abc;
    if(dir==NULL)
        return ;
    struct dirent* entity;
    entity=readdir(dir);
    vector<string>v1;
    int index = 0;
    while(entity!=NULL)
    {
        //printf("%s\n",entity->d_name);
        fileName=entity->d_name;
        string path = dirname + "/" + entity->d_name;
        s=fsize(path.c_str());
        char* symlinkpath=(char*)path.c_str();
        // strcat(path,dirname);
        // strcat(path,"/");
        // strcat(path,entity->d_name);
        a=getFileCreationTime(path.c_str());
        bo=permissions(path.c_str());
        co=user(path.c_str());
        d=group(path.c_str());
        ptr=realpath(symlinkpath,actualpath);
        // cout.setf(ios::right, ios::adjustfield);
        // cout.width(50);
        string fileName = entity->d_name;

        if(index == pointerIndex)   cout << ">>>\t";
        else    cout << "\t";

        printf("%-45s", fileName.c_str());
        printf("%s \t\t",co);
        printf("%s \t\t",d);
        cout << s << "KB\t\t";
        printf("%s \t\t",bo);
        cout << a;
        //sprintf(buffer,%d,s);
        // b=bo;
        // c=co;
        dl=fileName;
        pp=ptr;
        v1.push_back(pp);
        v1.push_back(fileName);
        // v1.push_back(c);
        // v1.push_back(dl);
        // v1.push_back(buffer);
        // v1.push_back(a);
         v.push_back(v1);
         v1.clear();

        // if((entity->d_type==DT_DIR) && (strcmp(entity->d_name,".")!=0) && (strcmp(entity->d_name,".,")!=0))
        // {
        //     cout<<"1"<<endl;
        //     char path[10000]={ 0 };
        //     char p[]={ "/"};
        //     char c[]={ entity->d_name };
        //     strcat(path,dirname);
        //     strcat(path,"/");
        //     strcat(path,entity->d_name);
        //     openfile(path);
        // }
         entity=readdir(dir);
         index++;
    }
    m=index;
    closedir(dir);
}
int main()
{
    enableRawMode();
    RefreshScreen();
    kl="../Downloads";
    openfile(kl);
    char c;
    while(1)
    {
        c=getchar();
        if (c == '\033') { 
        c=getchar();
        c=getchar(); 
        switch(c) { 
        case 'A':
            if(pointerIndex==0)
                break;
            else
            {
                pointerIndex--;
                RefreshScreen();
                v.clear();
                openfile(kl);
            }
            break;
        case 'B':
            if(pointerIndex==m-1)
                break;
            else
            {
                pointerIndex++;
                RefreshScreen();
                v.clear();
                openfile(kl);
            }
            // code for arrow down
            break;
        case 'C':
            if(s3.size()==0)
                break;
        else
        {
            RefreshScreen();
            s1.push(kl);
            kl=s3.top();
            s3.pop();
            openfile(kl);
        }
            break;
        case 'D':
            if(s1.size()==0)
                break;
        else   
         {
            RefreshScreen();
            s3.push(kl);
            kl=s1.top();
            s1.pop();
            openfile(kl);
         }
            // code for arrow left
            break;
    }
        }
    else if(c==0x0A)
    {
        s1.push(kl);
        string path;
        path=v[pointerIndex][0];
        kl=path;
        pointerIndex=0;
        v.clear();
        RefreshScreen();
        openfile(path);
    }
    }

        

    // int n,m;
    // n=v.size();
    // m=v[0].size();
    // for(int i=0;i<n;i++)
    // {
    //     for(int j=0;j<m;j++)
    //     {
    //         cout<<v[i][j]<<" ";
    //     }
    
    //      cout<<endl;
    // }
    return 0;
}