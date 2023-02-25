
#include "server.h"

using namespace std;
// int user_line[MAX_USER]={0} ; //每個user在shell裡面跑第幾行;
// int user_p[MAX_USER][MAX_USER][2] = {0};
// int user_pipe_check[MAX_USER][MAX_USER]= {0};
// int user[MAX_USER]={0};
// char* user_port[MAX_USER] = {0};
// char* user_name[MAX_USER] ={0};
// char* user_ip[MAX_USER]={0};


Pipe* global_p;
SEM* sem;
SHM* sh;
int main(int argc,char* argv[]){
    char buffer[1024];
    int user_n = 1;
    struct sockaddr_in serverAddr,Addr;
    //struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    char* ipstr;
    fd_set rfds,afds; //read,alive
    int msock,fd;
    int nfds = sizeof(afds);
    int flag =1,len = sizeof(int);
    stringstream ss;
    global_p = new Pipe();
    sem = new SEM();
    sh = new SHM(global_p,sem);


    // S = new Server(global_p);
    msock = socket(PF_INET, SOCK_STREAM, 0);
    int portNum = atoi(argv[1]);
    // cout<<portNum<<endl;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.2.11");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    if(setsockopt(msock,SOL_SOCKET,SO_REUSEADDR,&flag,len)==-1){
        perror("setsockopt");
        exit(1);
    }
    bind(msock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)); //set server socket

    if(listen(msock,MAX_USER)==0)
        printf("Server Listening\n");
    else
        printf("Error\n");
    
    FD_ZERO(&afds);
    FD_SET(msock,&afds);
    FD_SET(0,&afds);
    // for(int i=1;i<=MAX_USER;i++) user[i]=-1;
    // signal(SIGINT,handler_end);
    signal(SIGUSR1,handler);
    signal(SIGCHLD,SIG_IGN);
    signal(SIGINT,handler_end);
    while(1){
        memcpy(&rfds,&afds,sizeof(rfds));
        if(select(nfds,&rfds,(fd_set*)0,(fd_set*)0,(struct timeval *)0)<0)
        {
            fprintf(stderr,"select is wrong\n");
        }
        if(FD_ISSET(0,&rfds)){
            char in[10];
            read(0,in,sizeof(in));
            if(strncmp(in,"exit",4))
                continue;
            close(msock);
            break;
        }
        if(FD_ISSET(msock,&rfds)){
            int ssock; //slave 
            addr_size = sizeof(serverAddr);
            ssock = accept(msock, (struct sockaddr *) &serverAddr, &addr_size);
            getpeername(ssock,(struct sockaddr*)&serverAddr,&addr_size);
            if(ssock<0){
                fprintf(stderr,"accept is worng !\n");
                continue;
            }
            
            user_n = sh->setUserFdtable(ssock);//store slave fd in user table;
            
            if(user_n<0) {
                cerr<<"error in user fd table\n";
                continue;
            }
            string port_ch(to_string(ntohs((&serverAddr)->sin_port)));
            string init_name = "(no name)";
            string ip_name = inet_ntoa(serverAddr.sin_addr);

            sh->set_user(port_ch,init_name,ip_name,user_n);
            
            
            // FD_SET(ssock,&afds); //set ssock is alive
            pid_t client_process = fork();
            if(!client_process){
                sh->npshell(ssock,user_n);
                // cout<<" logout !\n";
                sh->logout(user_n);
                // cout<<" free user\n";
                sh->free_user(user_n);
                close(ssock);
                exit(0);
            }else{
                sh->set_pid(client_process,user_n);
                close(ssock);
            }
        }

        /*deal with the events of clients*/
        // for(int f=0;f<nfds;f++){
        //     if(f!=msock && FD_ISSET(f,&rfds)) // event happen
        //     {
        //         if(f==0) return 0;
        //         if(S->chat_cmd(f)<0){
        //             int ff = S->find_user_id(f);
        //             S->clean_user_info(ff,f);
        //             FD_CLR(f,&afds);
        //         }
        //     }
        // }
    }
    close(msock);
    delete sh;
    delete sem;
    delete global_p;
    return 0;
}
void handler(int signo)
{
    
    // int shm_id = shmget(sharemem,sizeof(SHM),0666);
    // SHM *sh = (SHM*)shmat(shm_id,NULL,0);
    sh->handle();
    return ;
}