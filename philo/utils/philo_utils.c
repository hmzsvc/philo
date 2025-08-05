/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:55:12 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/05 17:10:05 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/error.h"
#include "../lib/philo.h"
#include <unistd.h>

static void	one_philo_handle(t_philo *philo)
{
	if (philo->data->philo_count == 1)
	{
		pthread_mutex_lock(philo->left_fork);
		print(philo, "has taken a fork");
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_lock(&philo->data->death_mutex);
		philo->data->is_dead = 1;
		pthread_mutex_unlock(&philo->data->death_mutex);
	}
}

static void	*philo_process(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (1)
	{
		if (check_start_flag(philo))
			break ;
		usleep(100);
	}
	one_philo_handle(philo);
	sync_philo_start(philo);
	while (!check_dead(philo))
	{
		if (philo->data->must_eat != -1 && check_meal_goal(philo))
			break ;
		if (handle_dead(philo))
			break ;
		philo_take_fork(philo);
		philo_eat(philo);
		if (philo->right_fork_bool)
			pthread_mutex_unlock(philo->right_fork);
		philo->right_fork_bool = 0;
		if (philo->left_fork_bool)
			pthread_mutex_unlock(philo->left_fork);
		philo->left_fork_bool = 0;
		if (philo->data->must_eat != -1 && check_meal_goal(philo))
			break ;
		if (handle_dead(philo))
			break ;
		philo_sleep(philo);
		if (handle_dead(philo))
			break ;
		philo_thinking(philo);
	}
	return (NULL);
}

void	initialize_table(t_table *data, char *argv[], int argc)
{
	int	i;

	i = 0;
	parse_args(argv, data, argc);
	data->forks = NULL;
	data->philos = NULL;
	data->is_dead = 0;
	data->dead_index = -1;
	data->start_flag = 0;
	data->must_meal_loop = 0;
	data->philos = ft_calloc(sizeof(t_philo), data->philo_count);
	handle_error(data, ERR_MALLOC_FAIL, data->philos);
	while (i < data->philo_count)
	{
		data->philos[i].identity = i + 1;
		data->philos[i].data = data;
		data->philos[i].last_meal_time = get_time_in_ms();
		data->philos[i].eat_count = 0;
		data->philos[i].left_fork_bool = 0;
		data->philos[i].right_fork_bool = 0;
		i++;
	}
	if ((data->philo_count != data->philos[i - 1].identity))
		handle_error(data, ERR_THREAD_FAIL, NULL);
}
void	create_philo(t_table *data)
{
	int	i;

	i = -1;
	while (++i < data->philo_count)
	{
		if (pthread_create(&data->philos[i].thread, NULL, philo_process,
				&data->philos[i]))
		{
			pthread_mutex_lock(&data->start_flag_mutex);
			data->start_flag = 1;
			pthread_mutex_unlock(&data->start_flag_mutex);
			pthread_mutex_lock(&data->death_mutex);
			data->is_dead = 2;
			pthread_mutex_unlock(&data->death_mutex);
			break ;
		}
	}
	set_time(data);
	pthread_mutex_lock(&data->start_flag_mutex);
	data->start_flag = 1;
	pthread_mutex_unlock(&data->start_flag_mutex);
}

void	initialize_forks(t_table *data)
{
	int	i;

	i = -1;
	handle_mutex_error(data, pthread_mutex_init(&data->death_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->start_flag_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->check_meal_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->print_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->must_meal_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->meal_mutex, NULL));
	data->forks = ft_calloc(data->philo_count, sizeof(pthread_mutex_t));
	handle_error(data, ERR_MALLOC_FAIL, data->forks);
	while (++i < data->philo_count)
	{
		handle_mutex_error(data, pthread_mutex_init(&data->forks[i], NULL));
		handle_mutex_error(data,
			pthread_mutex_init(&data->philos[i].eat_count_mutex, NULL));
	}
	i = -1;
	while (++i < data->philo_count)
	{
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->philo_count];
	}
}
