/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:55:31 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/06 14:45:20 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/error.h"
#include "../lib/philo.h"
#include <stdio.h>
#include <stdlib.h>

static void	destroy_mutex(t_table *data)
{
	int	i;

	i = -1;
	pthread_mutex_destroy(&data->check_meal_mutex);
	pthread_mutex_destroy(&data->death_mutex);
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->start_flag_mutex);
	i = -1;
	if (data->forks)
	{
		while (++i < data->philo_count)
		{
			pthread_mutex_destroy(&data->forks[i]);
		}
	}
}

void	reset_table(t_table *data)
{
	destroy_mutex(data);
	if (data->philos)
		free(data->philos);
	if (data->forks)
		free(data->forks);
}

void	handle_error(t_table *data, int err_code, void *str)
{
	if (str)
		return ;
	if (err_code == ERR_MALLOC_FAIL)
		printf("ERR_MALLOC_FAIL\n");
	else if (err_code == ERR_INVALID_ARG)
		printf("ERR_INVALID_ARG\n");
	else if (err_code == ERR_THREAD_FAIL)
		printf("ERR_THREAD_FAIL\n");
	else if (err_code == ERR_ATOI_FAIL)
		printf("ERR_ATOI_FAIL\n");
	if (data)
		reset_table(data);
	exit(1);
}

void	handle_mutex_error(t_table *data, int value)
{
	if (value == 0)
		return ;
	if (value != 0)
		printf("ERR_MUTEX_FAIL\n");
	if (data)
		reset_table(data);
	exit(1);
}

void	one_philo_handle(t_philo *philo)
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
