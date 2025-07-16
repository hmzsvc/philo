/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:30:32 by huozturk          #+#    #+#             */
/*   Updated: 2025/07/12 16:02:23 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"
#include <stdlib.h>
#include <stddef.h>

void	 philo_join(t_data *data)
{
	int	i;

	i = -1;
	
	pthread_join(
		data->monitor_philo,
		NULL);
	if (data->dead_index != -1)
		philo_dead(data->philos[data->dead_index]);
	
	while (++i < data->philo_count)  // Bekleme fonksiyonu olusturulacak
	{
		pthread_join(
			data->philos[i].thread,
			NULL);
	}
}

void	*ft_memset(void *b, int c, size_t len)
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