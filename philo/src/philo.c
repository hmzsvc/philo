/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:54:41 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/03 20:21:14 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../lib/philo.h"
#include <stdio.h>
#include <unistd.h>

void	ft_usleep(int wait_time, t_philo *philo) // neden ft_usleep?
{
	unsigned long long	time;

	time = get_time_in_ms();
	while (get_time_in_ms() - time < (unsigned long long)wait_time)
	{
		handle_dead(philo);
		usleep(100);
	}
}

int	main(int argc, char *argv[])
{
	t_table	data;

	if (argc == 5 || argc == 6)
	{
		initialize_table(&data, argv, argc);
		initialize_forks(&data);
		setup_philosopher_monitor(&data);
		create_philo(&data);
		philo_join(&data);
		reset_table(&data);
		return (0);
	}
	else
	{
		printf("ERR_INVALID_ARG\n");
		return (1);
	}
}
