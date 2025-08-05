/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:55:00 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/05 22:52:12 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../lib/philo.h"
//utilys ve monitoru birleştir ve uygun değişimleri yap 
static void	begin_wait(t_table *data)
{
	while (1)
	{
		pthread_mutex_lock(&data->start_flag_mutex);
			if (data->start_flag == 1)
			{
				pthread_mutex_unlock(&data->start_flag_mutex);	
				break ;
			}
		pthread_mutex_unlock(&data->start_flag_mutex);
	}
}

static void	set_death_status(t_table *data, int philo_index)
{
	pthread_mutex_lock(&data->death_mutex);
	data->is_dead = 1;
	data->dead_index = philo_index;
	pthread_mutex_unlock(&data->death_mutex);
}

static int	check_and_handle_death(t_table *data, int philo_index)
{
	pthread_mutex_lock(&data->meal_mutex);
	data->last_meal_philo = data->philos[philo_index].last_meal_time;
	pthread_mutex_unlock(&data->meal_mutex);
	if (get_time_in_ms() - data->last_meal_philo >= data->time_to_die)
	{
		set_death_status(data, philo_index);
		return (1);
	}
	else if (check_dead(data->philos))
		return (1);
	return (0);
}


static void	*watch_for_death(void *argv)
{
	t_table	*datas;
	int		i;

	datas = (t_table *)argv;
	begin_wait(datas);
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