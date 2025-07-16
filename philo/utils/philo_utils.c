/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:46:25 by huozturk          #+#    #+#             */
/*   Updated: 2025/07/14 15:39:06 by huozturk         ###   ########.fr       */
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
	
	data->forks = NULL;
	data->philos = NULL;
	data->is_dead = 0;
	data->dead_index = -1;
	data->philos = ft_calloc(sizeof(t_philo), data->philo_count);
	error_check(data, ERR_MALLOC_FAIL, data->philos);
	while (i < data->philo_count)
	{
		data->philos[i].id = i + 1;
		data->philos[i].data = data;
		data->philos[i].last_meal = get_time_in_ms();
		data->philos[i].eat_count = 0;
		i++;
	}
	if ((data->philo_count != data->philos[i - 1].id))
		error_check(data, ERR_THREAD_FAIL, NULL);
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

void	init_forks(t_data *data) // Mutexler destroy edilecek mi ?
{
	int	i;

	i = -1;
	error_check_mutex(data, pthread_mutex_init(&data->death_mutex, NULL));
	error_check_mutex(data, pthread_mutex_init(&data->start_flag_mutex, NULL));
	error_check_mutex(data, pthread_mutex_init(&data->check_meal_mutex, NULL));
	error_check_mutex(data, pthread_mutex_init(&data->print_mutex, NULL));
	data->forks = ft_calloc(data->philo_count, sizeof(pthread_mutex_t));
	error_check(data, ERR_MALLOC_FAIL, data->forks);
	while (++i < data->philo_count)
	{
		error_check_mutex(data, pthread_mutex_init(&data->forks[i], NULL));
		error_check_mutex(data, pthread_mutex_init(&data->philos[i].meal_mutex, NULL));
	}

	i = -1;
	while (++i < data->philo_count)
	{
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->philo_count];
	}
}
