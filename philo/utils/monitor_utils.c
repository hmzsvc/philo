/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:54:55 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/05 21:07:54 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



// #include "../lib/philo.h"

// static void	set_death_status(t_table *data, int philo_index)
// {
// 	pthread_mutex_lock(&data->death_mutex);
// 	data->is_dead = 1;
// 	data->dead_index = philo_index;
// 	pthread_mutex_unlock(&data->death_mutex);
// }

// int	check_and_handle_death(t_table *data, int philo_index)
// {
// 	pthread_mutex_lock(&data->meal_mutex);
// 	data->/*monitöre ait bir strcut yapısı kuralabilir*/last_meal_philo = data->philos[philo_index].last_meal_time;
// 	pthread_mutex_unlock(&data->meal_mutex);
// 	if (get_time_in_ms() - data->last_meal_philo >= data->time_to_die)
// 	{
// 		set_death_status(data, philo_index);
// 		return (1);
// 	}
// 	else if (check_dead(data->philos))
// 		return (1);
// 	return (0);
// }

// void	wait_start(t_table *data)
// {
// 	while (1)
// 	{
// 		pthread_mutex_lock(&data->start_flag_mutex);
// 			if (data->start_flag == 1)
// 			{
// 				pthread_mutex_unlock(&data->start_flag_mutex);	
// 				break ;
// 			}
// 		pthread_mutex_unlock(&data->start_flag_mutex);
// 	}
// }
