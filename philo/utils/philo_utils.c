/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:46:25 by huozturk          #+#    #+#             */
/*   Updated: 2025/07/08 15:05:07 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"
#include "../lib/error.h"
#include <stdlib.h>

void	*say_hello(void *arg)
{
	t_philo *philo;

	philo = (t_philo *)arg;

	while (1)
	{
		if (check_start_flag(philo))
			break;
		usleep(100); // ✅ CRITICAL: Prevent CPU spinning!
	}
	sync_philo_start(philo);
	while (!check_dead(philo))
	{
		philo_take_fork(philo);
		philo_eat(philo);
		// ✅ IMPORTANT: Unlock in reverse order for consistency
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		philo_sleep(philo);
		philo_thinking(philo);
	}
	return (NULL);
}


void	init_philo(t_data *data, char *argv[], int argc)
{
	int	i;
	
	i = 0;
	parse_args(argv, data, argc);
	data->philos = calloc(sizeof(t_philo), data->philo_count); //ft_calloc eklenecek
	error_check(data, ERR_MALLOC_FAIL, data->philos);
	while (i < data->philo_count)
	{
		data->philos[i].id = i + 1;
		data->philos[i].data = data;
		data->philos[i].last_meal = get_time_in_ms();
		data->philos[i].eat_count = 0;
		i++;
	}
	if ((data->philo_count != data->philos[i - 1].id)) // DÜZELECEK
	{
		printf("LAST_ID: %d\n", data->philos[i - 1].id);
		exit(1);
	}
}

void	create_philo(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->philo_count)
	{
		pthread_create(
			&data->philos[i].thread,
			NULL,
			say_hello,
			&data->philos[i]);
	}
}

void	init_forks(t_data *data)
{
	int	i;

	i = -1;
	data->forks = malloc(data->philo_count * sizeof(pthread_mutex_t)); //ft_calloc eklenecek
	error_check(data, ERR_MALLOC_FAIL, data->forks);
	
	// Initialize global mutexes
	// checkler eklenicek
	pthread_mutex_init(&data->death_mutex, NULL);
	pthread_mutex_init(&data->start_flag_mutex, NULL);
	pthread_mutex_init(&data->check_meal_mutex, NULL);
	pthread_mutex_init(&data->print_mutex, NULL);
	while (++i < data->philo_count)
	{
		pthread_mutex_init(&data->forks[i], NULL); // AÇILDI MI AÇILMADI MI CHECK
		pthread_mutex_init(&data->philos[i].meal_mutex, NULL);
		
	}

	i = -1;
	while (++i < data->philo_count)
	{
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->philo_count];
	}
}
