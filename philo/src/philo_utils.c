/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:55:12 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/09 15:30:38 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/error.h"
#include "../lib/philo.h"
#include <unistd.h>

static int	check_finish(t_philo *philo)
{
	if (philo->data->must_eat != -1 && check_meal_goal(philo))
		return (1);
	if (handle_dead(philo))
		return (1);
	return (0);
}

static void	run_philo_loop(t_philo *philo)
{
	while (!check_dead(philo))
	{
		if (check_finish(philo))
			break ;
		philo_take_fork(philo);
		philo_eat(philo);
		if (philo->right_fork_bool)
			pthread_mutex_unlock(philo->right_fork);
		philo->right_fork_bool = 0;
		if (philo->left_fork_bool)
			pthread_mutex_unlock(philo->left_fork);
		philo->left_fork_bool = 0;
		if (check_finish(philo))
			break ;
		philo_sleep(philo);
		if (handle_dead(philo))
			break ;
		philo_thinking(philo);
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
	single_philo_operation(philo);
	sync_philo_start(philo);
	run_philo_loop(philo);
	return (NULL);
}

static void	set_start_flag(t_table *data)
{
	pthread_mutex_lock(&data->start_flag_mutex);
	data->start_flag = 1;
	pthread_mutex_unlock(&data->start_flag_mutex);
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
			set_start_flag(data);
			pthread_mutex_lock(&data->death_mutex);
			data->is_dead = 2;
			pthread_mutex_unlock(&data->death_mutex);
			break ;
		}
	}
	set_time(data);
	set_start_flag(data);
}
