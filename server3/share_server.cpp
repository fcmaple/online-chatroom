#include "server.h"


int SHM::init_table(){
    sem->wait_sem(semType::CLIENT);
    sem->wait_sem(semType::SH);
    for(int i=0;i<MAX_USER;i++){
        sh->user[i]=-1;
        for(int j=0;j<MAX_USER;j++) sh->fifo_table[i][j]=-1;
    }
    sh->user_count =0;
    sem->wait_sem(semType::COUNT);
    sh->user_count_read=0;
    sem->release_sem(semType::COUNT);
    sem->release_sem(semType::CLIENT);
    sem->release_sem(semType::SH);
    return 0;
}
int SHM::setUserFdtable(int fd){
    sem->wait_sem(semType::SH_CLIENT);
    int out = -1;
    for(int i=1;i<MAX_USER;i++){
        if(sh->user[i]!=-1) continue;
        sh->user[i]=fd;
        sh->user_count++;
        out = i;
        break;
    }
    sem->release_sem(semType::SH_CLIENT);
    return out;
}
void SHM::wel_mes(int user_id){
    cout<<"****************************************\n";
    cout<<"** Welcome to the information server. **\n";
    cout<<"****************************************\n";
    sem->wait_sem(semType::CLIENT);
    sh->user_in = user_id;
    sh->event = 1;
    for(int q=1;q<MAX_USER;q++){
        if(sh->user[q]!=-1) kill(sh->pidd[q],SIGUSR1);
    }
    while(sh->user_count_read<sh->user_count);
    sem->wait_sem(semType::SH);
    sh->user_in =0;
    sh->event =0;
    sem->wait_sem(semType::COUNT);
    sh->user_count_read=0;
    sem->release_sem(semType::COUNT);
    sem->release_sem(semType::CLIENT);
    sem->release_sem(semType::SH);

    return ;

}
int SHM::set_user(string pport,string nname,string iip,int user_id){
    sem->wait_sem(semType::CLIENT);
    memcpy(sh->name[user_id],nname.c_str(),nname.size()+5);
    memcpy(sh->port[user_id],pport.c_str(),pport.size()+5);
    void* c = memcpy(sh->ip[user_id],iip.c_str(),iip.size()+5);
    // printf("mem cpy %s\n",(char*)c);
    sem->release_sem(semType::CLIENT);
    return 1;
}
void SHM::initset()
{
    setenv("PATH","bin:.",1);
    
}
int SHM::print_env(string var){
    char *p = getenv((char*)var.data());
    if(p==NULL) return 0;
    else cout<<p<<endl;
    return 1;
}
int SHM::set_env(string var, string val){
    int p = setenv((char*)var.data(),(char*)val.data(),1);
    if(p<0) fprintf(stderr,"Set env error !\n");
    return 1;
}
int SHM::free_user(int user_id){

    sem->wait_sem(semType::SH);
    sem->wait_sem(semType::CLIENT);
    memset(sh->ip[user_id],'\0',MAX_USER);
    memset(sh->port[user_id],'\0',MAX_USER);
    memset(sh->name[user_id],'\0',MAX_USER);
    sh->user[user_id] = -1;
    sh->pidd[user_id] = -1;
    for(int q=1;q<MAX_USER;q++){
        if(sh->fifo_table[user_id][q] != -1){
            memset(sh->user_mes[user_id][q],'\0',MAX_WORDS_IN_LINE);
            close(sh->fifo_table[user_id][q]);
            sh->fifo_table[user_id][q] = -1;
        }   
    }
    for(int q=1;q<MAX_USER;q++){
        
        if(sh->fifo_table[q][user_id] != -1){
            memset(sh->user_mes[q][user_id],'\0',MAX_WORDS_IN_LINE);
            close(sh->fifo_table[q][user_id]);
            sh->fifo_table[q][user_id] = -1;
        }   
    }
    sem->release_sem(semType::CLIENT);
    sem->release_sem(semType::SH);
    return 1;
}
int SHM::logout(int user_id){
    sem->wait_sem(semType::CLIENT);
    sh->user_out = user_id;
    sh->me = user_id;
    sh->event = 2;
    for(int q=1;q<MAX_USER;q++)
        if(sh->user[q]!=-1 && q!=user_id) kill(sh->pidd[q],SIGUSR1);
    while(sh->user_count_read<sh->user_count-1);
    sem->wait_sem(semType::SH);
    sh->user_out=0;
    sh->event=0;
    sh->me=0;
    sem->wait_sem(semType::COUNT);
    sh->user_count_read=0;
    sem->release_sem(semType::COUNT);
    sh->user_count -=1;
    sem->release_sem(semType::CLIENT);
    sem->release_sem(semType::SH);

    return 0;
}
int SHM::specialEvent(Command com,int user_id){
    vector<vector<string>> command = com.get_command();
    for(auto cmd:command){
        if(cmd[0]=="printenv"){
            print_env(cmd[1]);
            return 0;
        }else if(cmd[0]=="setenv"){
            set_env(cmd[1],cmd[2]);
            return 0;
        }else if(cmd[0]=="exit"){
            // exitshell(user_id);
            return -1;
        }else if(cmd[0]=="name"){
            sem->wait_sem(semType::SH);
            for(int q=1;q<MAX_USER;q++){
                if(!strcmp(sh->name[q],cmd[1].c_str())){
                    printf("*** User '%s' already exists. ***\n",sh->name[q]);
                    sem->release_sem(semType::SH);
                    return 0;
                }
            }
            sem->release_sem(semType::SH);
            //if name check pass 
            sem->wait_sem(semType::CLIENT);
            sh->newn = user_id;
            sh->event=3;
            memset(sh->name[user_id],'\0',MAX_USER);
            memcpy(sh->name[user_id],cmd[1].c_str(),cmd[1].size());
            for(int q=1;q<MAX_USER;q++){
                if(sh->user[q]==-1) continue;
                kill(sh->pidd[q],SIGUSR1);
            }
            while(sh->user_count_read<sh->user_count);
            sem->wait_sem(semType::SH);
            sh->newn = 0;
            sh->event = 0;
            sem->wait_sem(semType::COUNT);
            sh->user_count_read=0;
            sem->release_sem(semType::COUNT);
            sem->release_sem(semType::CLIENT);
            sem->release_sem(semType::SH);
            return 0;

        }else if(cmd[0]=="who"){
            printf("<ID>    <nickname>  <IP:port>   <indicate me>\n");
            for(int q=1;q<MAX_USER;q++){
                if(sh->user[q] !=-1){
                    printf("%d  %s  %s:%s   ",q,sh->name[q],sh->ip[q],sh->port[q]);
                    if(q==user_id) printf("<-me\n");
                    else printf("\n");
                }
            }
            return 0;
        }else if(cmd[0]=="yell"){
            string mess;
            for(int i=1;i<cmd.size();i++){
                mess += cmd[i];
                if(i!=cmd.size()-1) mess+=" ";
            }
            sem->wait_sem(semType::CLIENT);
            sh->event=5;
            sh->yell_user = user_id;
            memcpy(sh->yellmes,mess.c_str(),mess.size());
            for(int q=1;q<MAX_USER;q++){
                if(sh->user[q]!=-1) kill(sh->pidd[q],SIGUSR1);
            }
            while(sh->user_count_read<sh->user_count);
            sem->wait_sem(semType::SH);
            sh->event = 0;
            sh->yell_user = 0;
            sem->wait_sem(semType::COUNT);
            sh->user_count_read=0;
            sem->release_sem(semType::COUNT);
            memset(sh->yellmes,'\0',MAX_WORDS_IN_LINE);
            sem->release_sem(semType::CLIENT);
            sem->release_sem(semType::SH);
            return 0;
        }else if(cmd[0]=="tell"){
            int receiver = atoi(cmd[1].c_str());
            string mess;
            for(int i=2;i<cmd.size();i++){
                mess += cmd[i];
                if(i!=cmd.size()-1) mess+=" ";
            }
            sem->wait_sem(semType::SH);
            if(sh->user[receiver]==-1){
                printf("*** Error: user #%d does not exist yet. ***\n",receiver);
                sem->release_sem(semType::SH);
            }
            sem->release_sem(semType::SH);
            sem->wait_sem(semType::CLIENT);
            sh->event=4;
            sh->tell_user = user_id;
            memcpy(sh->tellmes,mess.c_str(),mess.size());
            kill(sh->pidd[receiver],SIGUSR1);
            while(sh->user_count_read<1);
            sem->wait_sem(semType::SH);
            sh->tell_user=0;
            sh->event=0;
            sem->wait_sem(semType::COUNT);
            sh->user_count_read=0;
            sem->release_sem(semType::COUNT);
            memset(sh->tellmes,'\0',MAX_WORDS_IN_LINE);
            sem->release_sem(semType::CLIENT);
            sem->release_sem(semType::SH);
            return 0;
        }
    }
    return 1;
}
int SHM::set_pid(pid_t pp,int user_id){
    sem->wait_sem(semType::SH);
    sem->wait_sem(semType::CLIENT);
    sh->pidd[user_id]=pp;
    sem->release_sem(semType::CLIENT);
    sem->release_sem(semType::SH);
    return 1;
}

void SHM::handle(){
    int Event  =  sh->event;
    switch(Event){
        //printf("sh event %d\n",sh->event);
        case 0:
            cout<<"fault\n";
            break;
        case 1: //login
            sem->wait_sem(semType::SH);
            printf("*** User '%s' entered from %s:%s. ***\n",sh->name[sh->user_in],sh->ip[sh->user_in],sh->port[sh->user_in]);
            sem->wait_sem(semType::COUNT);
            sh->user_count_read +=1;
            sem->release_sem(semType::COUNT);
            sem->release_sem(semType::SH);
            break;
        case 2:
            sem->wait_sem(semType::SH);
            printf("*** User '%s' left. ***\n",sh->name[sh->user_out]);
            //if (!semaphore_p()) exit(EXIT_FAILURE);
            sem->wait_sem(semType::COUNT);
            sh->user_count_read +=1;

            sem->release_sem(semType::COUNT);
            sem->release_sem(semType::SH);
            //if (!semaphore_v()) exit(EXIT_FAILURE);
            break;
        case 3: //rename
            sem->wait_sem(semType::SH);
            printf("*** User from %s:%s is named '%s'. ***\n",sh->ip[sh->newn],sh->port[sh->newn],sh->name[sh->newn]);
            sh->user_count_read +=1;
            sem->release_sem(semType::SH);
            break;

        case 4:
            sem->wait_sem(semType::SH);
            printf("*** %s told you ***: %s\n",sh->name[sh->tell_user],sh->tellmes);
            sh->user_count_read +=1;
            sem->release_sem(semType::SH);
            break;
        case 5:
            sem->wait_sem(semType::SH);
            printf("*** %s yelled ***: %s\n",sh->name[sh->yell_user],sh->yellmes);
            sem->wait_sem(semType::COUNT);
            sh->user_count_read +=1;
            sem->release_sem(semType::COUNT);
            sem->release_sem(semType::SH);
            break;
        case 6: //user pipe boardcast
            sem->wait_sem(semType::SH);
            printf("*** %s (#%d) just piped '%s' to %s (#%d) ***\n",sh->name[sh->user_pipe_sender],sh->user_pipe_sender,
            sh->user_mes[sh->user_pipe_sender][sh->user_pipe_rec],sh->name[sh->user_pipe_rec],sh->user_pipe_rec);
            sem->wait_sem(semType::COUNT);
            sh->user_count_read +=1;
            sem->release_sem(semType::COUNT);
            sem->release_sem(semType::SH);
            break;
        case 7:
            sem->wait_sem(semType::SH);
            printf("*** %s (#%d) just received from %s (#%d) by '%s' ***\n",sh->name[sh->user_pipe_rec],
            sh->user_pipe_rec,sh->name[sh->user_pipe_sender],sh->user_pipe_sender,sh->user_mes[sh->user_pipe_sender][sh->user_pipe_rec]);
            sem->wait_sem(semType::COUNT);
            sh->user_count_read +=1;
            sem->release_sem(semType::COUNT);
            sem->release_sem(semType::SH);
            break;
        case 8:
            string path = "user_pipe/";
            sem->wait_sem(semType::SH);
            string sender(to_string(sh->user_pipe_sender));
            string rec(to_string(sh->user_pipe_rec));
            path = path+sender+"_"+rec;
            sh->fifo_table[sh->user_pipe_sender][sh->user_pipe_rec] = open(path.c_str(),O_RDONLY|O_NONBLOCK);
            sem->wait_sem(semType::COUNT);
            sh->user_count_read+=1;
            sem->release_sem(semType::COUNT);
            sem->release_sem(semType::SH);
            break;
    }
    return ;
}
int SHM::sig(int id,int user_id,string str){
    if(!id) return 0;
    // sem->wait_sem(semType::SH);
    sem->wait_sem(semType::CLIENT);
    sh->event=6;
    sh->user_pipe_sender = user_id;
    sh->user_pipe_rec =  id;
    memcpy(sh->user_mes[user_id][id],str.c_str(),str.size());
    for(int q=1;q<MAX_USER;q++){
        if(sh->user[q]!=-1) kill(sh->pidd[q],SIGUSR1);
    }
    while(sh->user_count_read<sh->user_count);

    sem->wait_sem(semType::SH);
    sh->mes_come_table[user_id][id] = 1; 
    sh->event = 0;
    sh->user_pipe_sender = 0;
    sh->user_pipe_rec = 0;
    sem->wait_sem(semType::COUNT);
    sh->user_count_read=0;
    sem->release_sem(semType::COUNT);
    sem->release_sem(semType::CLIENT);
    sem->release_sem(semType::SH);
    return 1;
}
pair<int,int> SHM::userPipe(string str,int user_id){
    int rev = 0;
    int send = 0;
    for(int i=0;i<str.size();i++){
        if(str[i]=='<'){
            int j=i+1;
            while(j<str.size() && str[j]>='0' && str[j]<='9'){
                rev=10*rev+(str[j]-'0');
                j++;
            }
        }
        if(str[i]=='>'){
            int j=i+1;
            while(j<str.size() && str[j]>='0' && str[j]<='9'){
                send=10*send+(str[j]-'0');
                j++;
            }
        }
    }
    int send_fd = 0;
    int rev_fd = 0;
    sem->wait_sem(semType::SH);
    if(rev>0 && rev<MAX_USER && sh->user[rev]!=-1 && sh->fifo_table[rev][user_id]!=-1){
        sem->release_sem(semType::SH);
        sem->wait_sem(semType::CLIENT);
        sh->event = 7;
        sh->user_pipe_sender = rev;
        sh->user_pipe_rec = user_id;
        memcpy(sh->user_mes[rev][user_id],str.c_str(),str.size());
        for(int q=1;q<MAX_USER;q++)
            if(sh->user[q]!=-1) kill(sh->pidd[q],SIGUSR1);
        while(sh->user_count_read<sh->user_count);
        sem->wait_sem(semType::SH);
        rev_fd = sh->fifo_table[rev][user_id];
        sh->mes_come_table[rev][user_id]=0;
        memset(sh->user_mes[rev][user_id],'\0',MAX_WORDS_IN_LINE);
        sh->event = 0 ;
        sh->user_pipe_sender=0;
        sh->fifo_table[rev][user_id]=-1;
        sh->user_pipe_rec = 0;
        sem->wait_sem(semType::COUNT);
        sh->user_count_read=0;
        sem->release_sem(semType::COUNT);
        sem->release_sem(semType::CLIENT);
        sem->release_sem(semType::SH);

    }else if( rev>0 &&(rev>MAX_USER || sh->user[rev]==-1)){
        sem->release_sem(semType::SH);
        rev_fd = -1;
    }else if(sh->fifo_table[rev][user_id]==-1 && rev>0){
        sem->release_sem(semType::SH);
        rev_fd = -2;
    }else sem->release_sem(semType::SH);

    sem->wait_sem(semType::SH);
    if(send>0&&send<MAX_USER && sh->user[send]!=-1 && sh->fifo_table[user_id][send]==-1){
        sem->release_sem(semType::SH);
        string path = "user_pipe/"+to_string(user_id)+"_"+to_string(send);
        if(mkfifo(path.c_str(),0666)<0 && errno!=EEXIST) perror("Create FIFO Failed!\n");
        sem->wait_sem(semType::CLIENT);
        sh->event=8;
        sh->user_pipe_sender=user_id;
        sh->user_pipe_rec = send;
        kill(sh->pidd[send],SIGUSR1);
        while(sh->user_count_read<1);
        // sem->wait_sem(semType::SH);
        sh->event = 0;
        sh->user_pipe_sender=0;
        sh->user_pipe_rec=0;
        sem->wait_sem(semType::COUNT);
        sh->user_count_read = 0;
        sem->release_sem(semType::COUNT);
        sem->release_sem(semType::CLIENT);
        // sem->release_sem(semType::SH);
        send_fd = open(path.c_str(),O_WRONLY);
    }else if(send>0 && ( send>MAX_USER||sh->user[send]==-1) ){
        sem->release_sem(semType::SH);
        send_fd=-1;
    }else if(sh->fifo_table[user_id][send]!=-1 && send){
        sem->release_sem(semType::SH);
        send_fd=-2;
    }else sem->release_sem(semType::SH);
    //final need to reset fifo table fd
    return {send_fd,rev_fd};
}
int close_fifo(int res){
    if(res)
        close(res);
    return 1;
}
SHM::SHM(Pipe* p,SEM* s){
    shm_id = shmget(sharemem,sizeof(SHMEM),IPC_CREAT | 0666);
    sh = (SHMEM*)shmat(shm_id,NULL,0);
    this->P = p;
    this->sem = s;
    init_table();
}
SHM::~SHM(){
    shmctl(shm_id, IPC_RMID, NULL); 
}

/*
wait client (can write client & event , everyone can read)
wait sh -> shared mem  (can read/write sh)
wait count (can write user_count_read,everyone can read)
signal -> send every client
*/
void handler_end(int signo)
{
    int shm_id = shmget(sharemem,sizeof(SHM),0666);
    shmctl(shm_id, IPC_RMID, NULL);
    exit(0);
    return;
}
/* signal*/
