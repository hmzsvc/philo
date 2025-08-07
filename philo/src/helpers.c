/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:54:50 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/07 12:00:29 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void	philo_join(t_table *data)
{
	int	i;

	i = -1;
	pthread_join(data->monitor_philo, NULL);
	if (data->dead_index != -1)
		philo_dead(data->philos[data->dead_index]);
	else if (data->philo_count == 1 && data->is_dead == 1)
		philo_dead(data->philos[0]);
	while (++i < data->philo_count)
	{
		pthread_join(data->philos[i].thread, NULL);
	}
}
void	ft_usleep(int wait_time, t_philo *philo) // neden ft_usleep?
{
	unsigned long long time;

	time = get_time_in_ms();
	while (get_time_in_ms() - time < (unsigned long long)wait_time)
	{
		handle_dead(philo);
		usleep(100);
	}
}

void	*ft_calloc(size_t count, size_t size)
{
	void			*memory;
	unsigned char	*ptr;
	size_t			total_size;

	total_size = count * size;
	memory = malloc(total_size);
	if (memory == NULL)
		return (NULL);
	// ft_memset'in içeriği doğrudan buraya entegre edildi
	ptr = (unsigned char *)memory;
	while (total_size > 0)
	{
		*ptr = 0;
		ptr++;
		total_size--;
	}
	return (memory);
}

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	set_time(t_table *data)
{
	data->start_time = get_time_in_ms();
}
