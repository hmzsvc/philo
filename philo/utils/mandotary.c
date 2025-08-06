/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mandotary.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 12:42:28 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/06 12:48:33 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"
#include <stdio.h>
#include <unistd.h>

void	philo_eat(t_philo *philo)
{
	last_meal_added(philo);
	print(philo, "is eating");
	pthread_mutex_lock(&philo->eat_count_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->eat_count_mutex);
	ft_usleep(philo->data->time_to_eat, philo);
}

void	philo_sleep(t_philo *philo)
{
	print(philo, "is sleeping");
	ft_usleep(philo->data->time_to_sleep, philo);
}

void	philo_thinking(t_philo *philo)
{
	print(philo, "is thinking");
	usleep(philo->data->time_to_die - (philo->data->time_to_eat
			+ philo->data->time_to_sleep)); // BAKILACAK KONTROL ET!
}

void	philo_dead(t_philo philo)
{
	pthread_mutex_lock(&philo.data->print_mutex);
	usleep(50);
	printf("%lld %d died\n", get_time_in_ms() - philo.data->start_time,
		philo.identity);
	pthread_mutex_unlock(&philo.data->print_mutex);
}

void	philo_take_fork(t_philo *philo)
{
	if (philo->identity % 2 == 0)
	{
		pthread_mutex_lock(philo->left_fork);
		philo->left_fork_bool = 1;
		print(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		philo->right_fork_bool = 1;
		print(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		philo->right_fork_bool = 1;
		print(philo, "has taken a fork");
		usleep(20);
		pthread_mutex_lock(philo->left_fork);
		philo->left_fork_bool = 1;
		print(philo, "has taken a fork");
	}
}
