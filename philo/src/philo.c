/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:46:28 by huozturk          #+#    #+#             */
/*   Updated: 2025/07/14 16:25:15 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h" 
#include <stdio.h>

long long	get_time_in_ms(void) // ARAŞTIRILACAK
{
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	set_time(t_data *data)
{
	data->start_time = get_time_in_ms();
	data->start_flag = 0;
}

int	main(int argc, char *argv[])
{
	t_data	data;

	if (argc == 5 || argc == 6)
	{
		init_philo(&data, argv, argc);
		init_forks(&data);
		monitor_philo(&data);
		set_time(&data);
		create_philo(&data);
		philo_join(&data);
		cleanup(&data);
		return (0);
	}
	else
	{
		fprintf(stderr, "ERR_INVALID_ARG\n");
		return (1);
	}
}
