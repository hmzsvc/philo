/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:07:58 by huozturk          #+#    #+#             */
/*   Updated: 2025/07/11 16:59:54 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"

void	philo_eat(t_philo *philo)
{
	last_meal_added(philo); // last meal update
	handle_dead(philo); // Tek philo için yazıldı

	print(philo, "is eating");
	philo->eat_count++;
	usleep(philo->data->time_to_eat * 1000);
}

void	philo_sleep(t_philo *philo)
{
	print(philo, "is sleeping");
	usleep(philo->data->time_to_sleep * 1000);
}

void	philo_thinking(t_philo *philo)
{
	print(philo, "is thinking");
	
	// ✅ CRITICAL: For odd number of philosophers, add thinking time to prevent starvation
	if (philo->data->philo_count % 2 == 1 && philo->data->philo_count > 1)
	{
		// Calculate thinking time to ensure all philosophers get a chance
		int think_time = (philo->data->time_to_eat * 2) - philo->data->time_to_sleep;
		if (think_time > 0)
			usleep(think_time * 1000);
		else
			usleep(1000); // Minimum 1ms thinking time
	}
}

void	philo_dead(t_philo philo)
{
	pthread_mutex_lock(&philo.data->print_mutex);
	printf("%lld %d died\n", get_time_in_ms() - philo.data->start_time, philo.id);
	pthread_mutex_unlock(&philo.data->print_mutex);
}

void	philo_take_fork(t_philo *philo)
{
	handle_dead(philo);
	check_meal_goal(philo); // BURAYA EKLEDİM AMA THİNKİNG VE SLEEP DE YAZIYOR BAKILMASI GEREK

	if (philo->data->philo_count == 1) // Tek philo için yazıldı
	{
		pthread_mutex_lock(philo->left_fork);
		print(philo, "has taken a fork");
		pthread_mutex_unlock(philo->left_fork);
		philo->data->is_dead = 1;
		return ;
	}
	
	// ✅ DEADLOCK PREVENTION: Use address-based ordering for ALL counts > 1
	pthread_mutex_t *first_fork, *second_fork;
	
	if (philo->left_fork < philo->right_fork)
	{
		first_fork = philo->left_fork;
		second_fork = philo->right_fork;
	}
	else
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	
	pthread_mutex_lock(first_fork);
	print(philo, "has taken a fork");
	pthread_mutex_lock(second_fork);
	print(philo, "has taken a fork");
}
