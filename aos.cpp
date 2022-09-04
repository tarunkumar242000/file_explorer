#include<stdio.h>
#include<iostream>
#include<dirent.h>
#include<string.h>
#include <fstream>
//#include <experimental/filesystem>
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
#include<fcntl.h>
#include<sys/sendfile.h>
using namespace std;

vector<string>vv;
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



void deletedir(string dirname)
{
    DIR *dir=opendir(dirname.c_str());
    if(dir==NULL)
        return;
    struct dirent* entity;
    while((entity=readdir(dir)!=NULL))
    {
        if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->dname, "..") !=0)
        {
            string path= dirname + "/" + entity->d_name;
            deletedir(path);
        }
        else
        {
            string path = dirname + "/" + entity->d_name;
            if(strcmp(entity->d_name, ".") != 0 && strcmp(entity->dname, "..") !=0)
            {
                int fgh=remove(path.c_str());
                cout<<fgh<<endl;
            }
        }
    }
    closedir(dir);
    int fgg=remove(dirname.c_str());
    cout<<fgg<<endl;
}


void copy_file(string a, string b )
{
    int source = open(a.c_str(), O_RDONLY, 0);
    int dest = open(b.c_str(), O_WRONLY | O_CREAT /*| O_TRUNC/**/, 0644);

    struct stat stat_source;
    fstat(source, &stat_source);

    sendfile(dest, source, 0, stat_source.st_size);

    close(source);
    close(dest);
}


void copy_directory(string a, string b )
{
    int res;
    struct stat st = {0};
    if(stat(b.c_str(),&st)==-1)
    {
        res=mkdir(b.c_str(),0777);
    }
    DIR *dir=opendir(a.c_str());
    if(dir==NULL)
        return;
    struct dirent* entity;
    while((entity=readdir(dir))!=NULL)
    {
        string fileName;
        fileName=entity->d_name;
        if(fileName=="." || fileName=="..") continue;

        struct stat inode;
        stat((a+"/"+fileName).c_str(), &inode);

        if(S_ISDIR(inode.st_mode))
        {
            string path,path1;
            path=a+"/"+fileName;
            path1=b+"/"+fileName;
            copy_directory(path,path1);
        }
        else{
            string path,path1;
            path=a+"/"+fileName;
            path1=b+"/"+fileName;
            // cout<<"path: "<<path<<endl;
            // cout<<endl<<"path1: "<<path1<<endl;

            copy_file(path,path1);
        }
    }
closedir(dir);
}


bool resi=false;
string fat;
bool searchfile(string dirname,string fat)
{
    // dirname+="/";
    //cout<<endl<<dirname<<" "<<fat<<endl;
    DIR *dir=opendir(dirname.c_str());
    if(dir==NULL)
        return false;
    struct dirent* entity;
    while((entity=readdir(dir))!=NULL)
    {
            //cout<<endl<<dirname<<" "<<fat<<endl;
        string fileName;
        fileName=entity->d_name;
        if(fileName=="." || fileName=="..") continue;
         //cout<<fileName<<" ";

        if(fileName==fat)
        {
            return true;
        }
        struct stat inode;
        stat(dirname.c_str(), &inode);

        if(S_ISDIR(inode.st_mode))
        {
            string path;
            path=dirname+"/"+fileName;
           
            //cout<<endl<<"Rec Call:" <<path<<endl;
            if(searchfile(path,fat))    
                return true;
        }
    }
    closedir(dir);
    return false;
}


int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
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
        
        fileName=entity->d_name;
        string path = dirname + "/" + entity->d_name;
        s=fsize(path.c_str());
        char* symlinkpath=(char*)path.c_str();
       
        a=getFileCreationTime(path.c_str());
        bo=permissions(path.c_str());
        co=user(path.c_str());
        d=group(path.c_str());
        ptr=realpath(symlinkpath,actualpath);
        
        string fileName = entity->d_name;

        if(index == pointerIndex)   cout << ">>>\t";
        else    cout << "\t";

        printf("%-45s", fileName.c_str());
        printf("%s \t\t",co);
        printf("%s \t\t",d);
        cout << s << "KB\t\t";
        printf("%s \t\t",bo);
        cout << a;
       
        dl=fileName;
        pp=ptr;
        v1.push_back(pp);
        v1.push_back(fileName);
         v.push_back(v1);
         v1.clear();

        
         entity=readdir(dir);
         index++;
    }
    m=index;
    closedir(dir);
    cout<<endl<<endl<<endl;
    cout<<"NORMAL MODE  :";
    cout<<realpath(kl.c_str(),NULL)<<endl;
}
void openfile1(string dirname)
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

        fileName=entity->d_name;
        string path = dirname + "/" + entity->d_name;
        s=fsize(path.c_str());
        char* symlinkpath=(char*)path.c_str();
        
        a=getFileCreationTime(path.c_str());
        bo=permissions(path.c_str());
        co=user(path.c_str());
        d=group(path.c_str());
        ptr=realpath(symlinkpath,actualpath);
        
        string fileName = entity->d_name;

    

        printf("%-45s", fileName.c_str());
        printf("%s \t\t",co);
        printf("%s \t\t",d);
        cout << s << "KB\t\t";
        printf("%s \t\t",bo);
        cout << a;
        
        dl=fileName;
        pp=ptr;
        v1.push_back(pp);
        v1.push_back(fileName);
         v.push_back(v1);
         v1.clear();

        
         entity=readdir(dir);
         index++;
    }
    m=index;
    closedir(dir);
}

void command_line(void);


void normal_mode()
{
    openfile(kl);
    // cout<</n/n;
    // cout<</t/t<<"NORMAL MODE"<</t/t<<endl;
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
        if(s1.empty()==true)
        {s1.push(kl);
        string path;
        path=v[pointerIndex][0];
        struct stat file_st;
        stat(path.c_str(),&file_st);
         if(S_ISDIR(file_st.st_mode))
         {
            pointerIndex=0;
            v.clear();
            kl=path;
            RefreshScreen();
            openfile(path);
        }
        else{
            pid_t pid=fork();
            if(pid == 0)
             {execl("/usr/bin/bin/xdg-open", "xdg-open",path.c_str(),(char*)NULL);
                exit(1);
             }
         }
        }
    else if(s1.top()!=kl)
    {
        s1.push(kl);
        string path;
        path=v[pointerIndex][0];
        struct stat file_st;
        stat(path.c_str(),&file_st);
        if(S_ISDIR(file_st.st_mode))
        {
            pointerIndex=0;
            v.clear();
            kl=path;
            RefreshScreen();
            openfile(path);
        }
        else{
            pid_t pid=fork();
            if(pid == 0)
            {   
                execl("/usr/bin/xdg-open", "xdg-open",path.c_str(),(char*)NULL);
                exit(1);
            }
        }
    }
    }
    else if(c==58)
    {
        //RefreshScreen();
        disableRawMode();
        command_line();
        break;
    }
    else if(c==127)
    {
        s3.push(kl);
        while(!s1.empty())
            s1.pop();
        kl=kl+"/"+"..";
        RefreshScreen();
        openfile(kl);
    }
    else if(c==104)
    {
        s1.push(kl);
        // while(!s1.empty())
        //     s1.pop();
        const char *homedir;
        if((homedir = getenv("HOME"))==NULL)
        {
            homedir=getpwuid(getuid())->pw_dir;
        }
        kl=homedir;
        RefreshScreen();
        openfile(kl);
    }
    else if(c==113)
    {
        exit(0);
    }
    }
}
void command_line()
{
    RefreshScreen();
    openfile1(kl);
    cout<<endl<<endl<<endl;
    cout<<"COMMAND MODE"<<":  ";
    cout<<realpath(kl.c_str(),NULL)<<endl;
    string line,temp1,temp2,temp3;
    cout<<"$"<<" ";
    int n,i,ren;
    char cooa;
    const char* choio;
    while(getline(cin, line))
    {
        choio=line.c_str();
        (char*)choio;
        cooa=choio[0];
        cout<<"$"<<" ";
        n=line.size();
        i=0;
        temp2="";
        while(line[i]!=' ')
        {
            temp1=line[i];
            temp2+=temp1;
            i++;
        }
        i++;
        temp3=kl;
        if(temp2=="rename")
        {
            while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
            //cout<<vv[0]<<"  "<<vv[1]<<endl;
            temp3+="/"+vv[0];
            vv[0]=temp3;
            temp3="";
            temp3=kl;
            temp3+="/" +vv[1];
            vv[1]=temp3;
            ren=rename(vv[0].c_str(),vv[1].c_str());
            vv.clear();
            RefreshScreen();
            openfile1(kl);
            cout<<endl<<endl<<endl;
            cout<<"COMMAND MODE"<<":  ";
            cout<<realpath(kl.c_str(),NULL)<<endl;
            temp3="";
        }

        else if(temp2=="goto")
        {
            while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
                kl=vv[0];
                RefreshScreen();
                openfile1(kl);
                cout<<endl<<endl<<endl;
                cout<<"COMMAND MODE"<<":  ";
                cout<<realpath(kl.c_str(),NULL)<<endl;
                vv.clear();
        }
        else if(temp2=="create_dir")
        {
            while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
            vv[1]=realpath(vv[1].c_str(),NULL);
            temp3="";
            temp3+=vv[1];
            temp3+="/";
            temp3+=vv[0];
            int res;
            struct stat st = {0};
            if(stat(temp3.c_str(),&st)==-1)
            {
            res=mkdir(temp3.c_str(),0777);
            }
            //cout<<res<<endl;
            temp3="";
            RefreshScreen();
            openfile1(vv[1]);
            cout<<endl<<endl<<endl;
            cout<<"COMMAND MODE"<<":  ";
            cout<<realpath(vv[1].c_str(),NULL)<<endl;
             vv.clear();
        }
        else if(temp2=="create_file")
        {
             while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
            vv[1]=realpath(vv[1].c_str(),NULL);
            temp3="";
            temp3+=vv[1];
            temp3+="/";
            temp3+=vv[0];
            //int fd2 = open(temp3.c_str(),O_RDWR|O_CREAT,0777);
            int fd2 = open(temp3.c_str(),O_RDWR|O_CREAT,S_IRUSR|S_IRGRP|S_IROTH);
            if(fd2 != -1)
            {
                close(fd2);
            }
            //cout<<fd2<<endl;
            RefreshScreen();
            openfile1(vv[1]);
            cout<<endl<<endl<<endl;
            cout<<"COMMAND MODE"<<":  ";
            cout<<realpath(vv[1].c_str(),NULL)<<endl;
            vv.clear();
        }
        else if(temp2=="copy")
        {
            while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
            int k;
            k=vv.size();
            string tempo,temp4;
            vv[k-1]=realpath(vv[k-1].c_str(),NULL);
            tempo=vv[k-1];
            k--;
            i=0;
            while(k--)
            {
                temp4=tempo;
                temp3=kl;
                temp3+="/";
                temp3+=vv[i];
                temp4+="/";
                temp4+=vv[i];

                if(is_regular_file(temp3.c_str()))
                {
                        copy_file(temp3,temp4);
                        RefreshScreen();
                        openfile1(tempo);
                        cout<<endl<<endl<<endl;
                        cout<<"COMMAND MODE"<<":  ";
                        cout<<realpath(tempo.c_str(),NULL)<<endl;
                }
                else
                {
                    copy_directory(temp3,temp4);
                    RefreshScreen();
                    openfile1(tempo);
                    cout<<endl<<endl<<endl;
                    cout<<"COMMAND MODE"<<":  ";
                    cout<<realpath(tempo.c_str(),NULL)<<endl;
                }
                i++;
            }
            vv.clear();
        }
        else if(temp2=="move")
        {
            while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
            int k,i,rin;
            k=vv.size();
            string tempo,temp4;
            vv[k-1]=realpath(vv[k-1].c_str(),NULL);
            tempo=vv[k-1];
            k--;
            i=0;
            while(k--)
            {
                temp4=tempo;
                temp3=kl;
                temp3+="/";
                temp3+=vv[i];
                temp4+="/";
                temp4+=vv[i];
                rin=rename(temp3.c_str(),temp4.c_str());
                //cout<<rin<<endl;
                i++;
            }
            RefreshScreen();
            openfile1(tempo);
            cout<<endl<<endl<<endl;
            cout<<"COMMAND MODE"<<":  ";
            cout<<realpath(tempo.c_str(),NULL)<<endl;
            vv.clear();
        }
        else if(temp2=="search")
        {
             while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
            bool res;
            fat=vv[0];
            //cout<<fat<<endl;
            //cout<<endl<<"Dir to search: "<<kl<<endl;

            res=searchfile(kl,fat);
            if(res==true)
                cout<<"True"<<endl;
            else
                cout<<"False"<<endl;

            vv.clear();
            //usleep(100000000);
        }
        else if(temp2=="delete_file")
        {
             while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
            // string teemp,strr;
            // strr=vv[0];
            // int len,i;
            // len=strr.size();
            // i=len;
            // while(strr[i]!='/')
            // {
            //     teemp.push_back(strr[i]);
            //     i--;
            // }
            // reverse(teemp.begin(),teemp.end());
            vv[0]=realpath(vv[0].c_str(),NULL);
            int aa;
            if(access(vv[0].c_str(),F_OK)==0)
            {
                aa=remove(vv[0].c_str());
                cout<<aa<<endl;
            }
            vv.clear();
        }
        else if(temp2=="delete_dir")
        {
             while(i<n)
            {    
                temp2="";
                while((line[i]!=' ') && (i<n))
                {
                    temp1=line[i];
                    temp2+=temp1;
                    i++;
                }
                i++;
                vv.push_back(temp2);
            }
            vv[0]=realpath(vv[0].c_str(),NULL);
            deletedir(vv[0]);
            vv.clear();
        }
        else if(cooa==27)
        {
            enableRawMode();
            RefreshScreen();
            normal_mode();
        }
        else if(temp2=="quit")
        {
            exit(0);
        }
    }
}

int main()
{
    enableRawMode();
    RefreshScreen();
    kl="/home/coding/Documents";
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
        
    if(s1.empty()==true)
        {s1.push(kl);
        string path;
        path=v[pointerIndex][0];
        struct stat file_st;
        stat(path.c_str(),&file_st);
         if(S_ISDIR(file_st.st_mode))
         {
            pointerIndex=0;
            v.clear();
            kl=path;
            RefreshScreen();
            openfile(path);
        }
        else{
            pid_t pid=fork();
            if(pid == 0)
             {
                execl("/usr/bin/xdg-open", "xdg-open",path.c_str(),(char*)NULL);
                exit(1);
             }
         }
        }
    else if(s1.top()!=kl)
    {
        s1.push(kl);
        string path;
        path=v[pointerIndex][0];
        struct stat file_st;
        stat(path.c_str(),&file_st);
        if(S_ISDIR(file_st.st_mode))
        {
            pointerIndex=0;
            v.clear();
            kl=path;
            RefreshScreen();
            openfile(path);
        }
        else{
            pid_t pid=fork();
            if(pid==0)
            {   
                execl("/usr/bin/xdg-open", "xdg-open",path.c_str(),(char*)NULL);
                exit(1);
            }
        }
    }
    }
    else if(c==58)
    {
        //RefreshScreen();
        disableRawMode();
        command_line();
        break;
    }
    else if(c==127)
    {
        s3.push(kl);
        while(!s1.empty())
            s1.pop();
        kl=kl+"/"+"..";
        RefreshScreen();
        openfile(kl);
    }
    else if(c==104)
    {
        s1.push(kl);
        // while(!s1.empty())
        //     s1.pop();
        const char *homedir;
        if((homedir = getenv("HOME"))==NULL)
        {
            homedir=getpwuid(getuid())->pw_dir;
        }
        kl=homedir;
        RefreshScreen();
        openfile(kl);
    }
    else if(c==113)
    {
        return 0;
    }
    }

        
    return 0;
}