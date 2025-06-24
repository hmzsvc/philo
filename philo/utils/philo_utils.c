/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:46:25 by huozturk          #+#    #+#             */
/*   Updated: 2025/06/24 13:23:12 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"
#include "../lib/error.h"
#include <stdlib.h>

static void	*say_hello(void *argv) // Tek thread sayisinda olum senaryosunda process kapanmiyor cift sayilarda kapaniyor problem left | right forklarda buyuk ihtimalle
{
	t_philo *philo;

	philo = (t_philo *)argv;
	
	// start flag 1 olunca patlat
	while (1)
	{
		if (philo->data->start_flag)
			break;
	}
	
	if (philo->id % 2 != 0)
		usleep(100);

	while (!philo->data->is_dead) // Buraya mutex kontrollü koşul lazım
	{
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(philo->left_fork);
			pthread_mutex_lock(philo->right_fork);
		}
		else
		{
			pthread_mutex_lock(philo->left_fork);
			pthread_mutex_lock(philo->right_fork);
		}
		printf("thread_id: %d - philo_count: %d - just EAT! ms: %lld philo_is_dead: %d", philo->id, philo->data->philo_count, get_time_in_ms() - philo->data->start_time, philo->data->is_dead);

		pthread_mutex_lock(&philo->meal_mutex);
		philo->last_meal = get_time_in_ms(); // Mutex kullanılacak
		pthread_mutex_unlock(&philo->meal_mutex);
		
		usleep(philo->data->time_to_eat * 1000);
		if (philo->data->is_dead)
		{
			pthread_exit(NULL);
		} // Dead mutex eklenebilir.
		printf(" - last_meal: %d \n", get_time_in_ms() - philo->last_meal);
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);

		usleep(philo->data->time_to_sleep * 1000); // Uyuma kismi olacak
	}
}

void	init_philo(t_data *data, char *argv[])
{
	int	i;

	i = 0;
	if (ft_atoi(argv[1], &data->philo_count)
		|| ft_atoi(argv[2], &data->time_to_die)
		|| ft_atoi(argv[3], &data->time_to_eat)
		|| ft_atoi(argv[3], &data->time_to_sleep))
		error_check(data, ERR_INVALID_ARG, NULL);
	data->philos = malloc(sizeof(t_philo) * data->philo_count);
	error_check(data, ERR_MALLOC_FAIL, data->philos);
	while (i < data->philo_count)
	{
		data->philos[i].id = i + 1;
		data->philos[i].data = data;
		data->philos[i].last_meal = get_time_in_ms();
		i++;
	}
}

void	create_philo(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->philo_count) // Philo oluşup oluşmadı mı kontrol id ile yapılabilir
	{
		pthread_create(
			&data->philos[i].thread,
			NULL,
			say_hello,
			&data->philos[i]);
	}
}

void	monitor_test(void *argv)
{
	t_data	*datas;
	int	i;
	
	datas = (t_data *)argv;
	datas->start_flag = 1;
	while (1)
	{
		i = -1;
		while (++i < datas->philo_count)
		{
			pthread_mutex_lock(&datas->philos[i].meal_mutex);
			long long last = datas->philos[i].last_meal;
			if (get_time_in_ms() - last > datas->time_to_die)
			{
				pthread_mutex_lock(&datas->philos[i].dead_mutex); // DENEMEK GEREK OLMADI
				datas->is_dead = 1; // Buraya da mutex lazım(sanırım)
				// printf("\nlast_meal: %lld\n",get_time_in_ms() -  last);
				pthread_mutex_unlock(&datas->philos[i].dead_mutex);

				printf("\n    DEAD_ID: %d LAST_MEAL: %d\n", datas->philos[i].id, get_time_in_ms() -  last);
				pthread_exit(NULL);
			}
			pthread_mutex_unlock(&datas->philos[i].meal_mutex);
		}
	}
}

void	monitor_philo(t_data *data)
{
	pthread_create(
		&data->monitor_philo,
		NULL,
		monitor_test,
		&*data);
}

void	init_forks(t_data *data)
{
	int	i;

	i = -1;
	data->forks = malloc(data->philo_count * sizeof(pthread_mutex_t));
	error_check(data, ERR_MALLOC_FAIL, data->forks);

	while (++i < data->philo_count)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		pthread_mutex_init(&data->philos[i].meal_mutex, NULL);
		pthread_mutex_init(&data->philos[i].dead_mutex, NULL);
	}

	i = -1;
	while (++i < data->philo_count)
	{
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->philo_count];
	}
}