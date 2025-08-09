#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct s_threads t_threads;

typedef struct s_bank
{
    int bank;
    int process;
    int human_count;
    int start_flag;
    pthread_mutex_t human_mutex;
    pthread_mutex_t start_mutex;
    t_threads *human;
}t_bank;

typedef struct s_threads
{
    int id;
    pthread_t thread;
    t_bank *bank;
} t_threads;

void *process(void *arg)
{
    t_threads *human;
    human = (t_threads *)arg;
    while (1)
    {
        pthread_mutex_lock(&human->bank->start_mutex);
        if(human->bank->start_flag == 1)
        {
            pthread_mutex_unlock(&human->bank->start_mutex);
            break;
        }
        pthread_mutex_unlock(&human->bank->start_mutex);
    }    
    if (human->bank->start_flag == 1)
    
    pthread_mutex_lock(&human->bank->human_mutex);
    human->bank->bank -= human->bank->process;
    printf("bank acount = %d process id = %d\n", human->bank->bank, human->id);
    pthread_mutex_unlock(&human->bank->human_mutex);
}


int main()
{
    int i = -1;
    
    t_bank banker;
    banker.human_count = 5;
    banker.human = malloc(sizeof(t_threads) * banker.human_count);
    banker.bank = 2000;
    banker.process = 250;
    banker.start_flag = 0;

    pthread_mutex_init(&banker.human_mutex,NULL);
    pthread_mutex_init(&banker.start_mutex,NULL);

    while (++i < banker.human_count)
    {
        banker.human[i].id = i + 1;
        banker.human[i].bank = &banker;
    }
    
    i = -1;
    while (++i < banker.human_count)
    {
        pthread_create(&banker.human[i].thread, NULL, &process,&banker.human[i]);
        //printf("hüso burada amk \n");
        // pthread_join(banker.human[i].thread, NULL);
    }
    pthread_mutex_lock(&banker.start_mutex);
    banker.start_flag = 1;
    pthread_mutex_unlock(&banker.start_mutex);

    i = -1;
    while (++i < banker.human_count)
    {
        //pthread_create(&banker.human[i].thread, NULL, &process,&banker.human[i]);
        //printf("hüso burada amk \n");
        pthread_join(banker.human[i].thread, NULL);
    }
}