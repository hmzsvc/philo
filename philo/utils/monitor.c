/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:55:00 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/04 00:04:02 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../lib/philo.h"
//utilys ve monitoru birleştir ve uygun değişimleri yap 
static void	*watch_for_death(void *argv)
{
	t_table	*datas;
	int		i;

	datas = (t_table *)argv;
	wait_start(datas);
	while (1)
	{
		i = -1;
		while (++i < datas->philo_count)
		{
			if (check_and_handle_death(datas, i))
				return (NULL);
			pthread_mutex_lock(&datas->must_meal_mutex);
			if (datas->must_eat != -1 && (datas->must_meal_loop >= datas->philo_count))
			{
				datas->is_dead = 3;
				pthread_mutex_unlock(&datas->must_meal_mutex);
				return(NULL);
			}
			pthread_mutex_unlock(&datas->must_meal_mutex);
		}
	}
}

void	setup_philosopher_monitor(t_table *data)
{
	if (pthread_create(
		&data->monitor_philo,
		NULL,
		watch_for_death,
		&*data) != 0)
	{
		data->start_flag = 1;
		data->is_dead = 2;
	}
}
