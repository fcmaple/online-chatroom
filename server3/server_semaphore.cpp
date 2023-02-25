#include"server.h"
using namespace std;

SEM::SEM(){
    sem_init(&sem_sh,0,1);
    sem_init(&sem_client_id,0,1);
    sem_init(&sem_count,0,1);
}
SEM::~SEM(){
    sem_destroy(&sem_client_id);
    sem_destroy(&sem_count);
    sem_destroy(&sem_sh);
}
int SEM::wait_sem(semType type){
    switch (type)
    {
    case semType::SH:
        sem_wait(&sem_sh);
        break;
    case semType::COUNT:
        sem_wait(&sem_count);
        break;
    case semType::CLIENT:
        sem_wait(&sem_client_id);
        break;
    case semType::SH_CLIENT:
        sem_wait(&sem_sh);
        sem_wait(&sem_client_id);
        break;
    default:
        sem_wait(&sem_sh);
        sem_wait(&sem_client_id);
        break;
    }
    return 1;
}
int SEM::release_sem(semType type){
    switch (type)
    {
    case semType::SH:
        sem_post(&sem_sh);
        break;
    case semType::CLIENT:
        sem_post(&sem_client_id);
        break;
    case semType::COUNT:
        sem_post(&sem_count);
        break;
    case semType::SH_CLIENT:
        sem_post(&sem_sh);
        sem_post(&sem_client_id);
        break;
    default:
        break;
    }
    return 1;
}