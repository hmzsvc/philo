/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:07:47 by huozturk          #+#    #+#             */
/*   Updated: 2025/07/31 19:52:08 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"

static void	*monitor_process(void *argv)
{
	t_data	*datas;
	int		i;

	datas = (t_data *)argv;
	wait_start(datas);
	while (1)
	{
		i = -1;
		while (++i < datas->philo_count)
		{
			check_and_handle_death(datas, i);
			pthread_mutex_lock(&datas->must_meal_mutex);
			if (datas->must_eat != -1 && (datas->must_meal_num >= datas->philo_count))
			{
				datas->is_dead = 3;
				pthread_mutex_unlock(&datas->must_meal_mutex);
				pthread_exit(NULL);
			}
			pthread_mutex_unlock(&datas->must_meal_mutex);
		}
	}
}

void	monitor_philo_create(t_data *data)
{
	if (pthread_create(
		&data->monitor_philo,
		NULL,
		monitor_process,
		&*data))
	{
		data->start_flag = 1;
		data->is_dead = 2;
	}
}
