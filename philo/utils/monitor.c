/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:07:47 by huozturk          #+#    #+#             */
/*   Updated: 2025/07/11 17:00:16 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"

void	*monitor_test(void *argv)
{
	t_data	*datas;
	int	i;
	
	datas = (t_data *)argv;
	datas->start_flag = 1;

	while (1)
	{
		i = -1;
		while (++i < datas->philo_count)
		{
			pthread_mutex_lock(&datas->philos[i].meal_mutex);
			long long last = datas->philos[i].last_meal;
			pthread_mutex_unlock(&datas->philos[i].meal_mutex);

			if (get_time_in_ms() - last > datas->time_to_die)
			{
				pthread_mutex_lock(&datas->death_mutex);
				datas->is_dead = 1; // Now protected by mutex
				datas->dead_index = i;
				pthread_mutex_unlock(&datas->death_mutex);
				handle_dead(datas->philos);
				pthread_exit(NULL);
			}
			else
			{
				if (datas->must_eat == datas->philos[i].eat_count)
					pthread_exit(NULL);
			}
		}
		// ✅ PERFORMANCE: Add sleep to prevent CPU spinning with 200 philosophers
		usleep(1000); // 1ms polling - sufficient for death detection
	}
}

void	monitor_philo(t_data *data)
{
	pthread_create(
		&data->monitor_philo,
		NULL,
		monitor_test,
		&*data);
}