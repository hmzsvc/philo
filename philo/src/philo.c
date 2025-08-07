/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:54:41 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/07 15:53:07 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/error.h"
#include "../lib/philo.h"
#include <stdio.h>

static void	initialize_table(t_table *data, char *argv[], int argc);

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

static void	init_table(t_table *data)
{
	data->forks = NULL;
	data->philos = NULL;
	data->is_dead = 0;
	data->dead_index = -1;
	data->start_flag = 0;
	data->must_meal_loop = 0;
	data->philos = ft_calloc(sizeof(t_philo), data->philo_count);
}

static void	initialize_table(t_table *data, char *argv[], int argc)
{
	int	i;

	i = 0;
	parse_args(argv, data, argc);
	init_table(data);
	handle_error(data, ERR_MALLOC_FAIL, data->philos);
	while (i < data->philo_count)
	{
		data->philos[i].identity = i + 1;
		data->philos[i].data = data;
		data->philos[i].last_meal_time = get_time();
		data->philos[i].eat_count = 0;
		data->philos[i].left_fork_bool = 0;
		data->philos[i].right_fork_bool = 0;
		i++;
	}
	if ((data->philo_count != data->philos[i - 1].identity))
		handle_error(data, ERR_THREAD_FAIL, NULL);
}
