/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:30:32 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/01 19:52:37 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"
#include <stdlib.h>
#include <sys/time.h>

void	philo_join(t_data *data)
{
	int	i;

	i = -1;
	
	pthread_join(
		data->monitor_philo,
		NULL);
	if (data->dead_index != -1)
		philo_dead(data->philos[data->dead_index]);	
	else if (data->philo_count == 1 && data->is_dead == 1)
		philo_dead(data->philos[0]);	
	while (++i < data->philo_count)
	{
		pthread_join(
			data->philos[i].thread,
			NULL);
	}
}

static void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*str;

	str = (unsigned char *)b;
	while (len > 0)
	{
		*str = (unsigned char)c;
		len--;
		str++;
	}
	return (b);
}

void	*ft_calloc(size_t count, size_t size)
{
	void	*memory;

	memory = malloc(count * size);
	if (memory == NULL)
	{
		return (NULL);
	}
	ft_memset(memory, 0, size * count);
	return (memory);
}

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	set_time(t_data *data)
{
	data->start_time = get_time_in_ms();
}
