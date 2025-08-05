#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef struct s_philo t_philo;
// int		bank = 1000;
typedef struct s_bank
{
    int cuurent_bank;
    int money;
    t_philo *philo;

}		t_bank;

typedef struct s_philo
{
    int id;
    pthread_t threads[5];
    t_bank *philo_bank;

}   t_philo;


void	*toplama(void *hüso)
{
	t_bank	*data;
    usleep(1); 
	data = (t_bank *)hüso;
	data->cuurent_bank -= data->money;
	printf("işlemi yapan: %d banka sonucu: %d\n", data->philo->id, data->cuurent_bank);
    
}

int	main(void)
{
	t_bank	banker;
	int				i;
    banker.cuurent_bank = 2000;
    banker.money = 250;
    t_philo filo;
    banker.philo = &filo;

    i = -1;
	while (++i < 5)
	{
        filo.id = i;
		pthread_create(&filo.threads[i], NULL, toplama, &banker);
        pthread_mutex_lock()
        pthread_join(filo.threads[i], NULL);
        
	}
    // i = -1;
    // while (++i < 5)
    // {
        
    // }
	return (0);
}