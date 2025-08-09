/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:54:41 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/09 15:29:13 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/error.h"
#include "../lib/philo.h"

static void	initialize_table(t_table *data, char *argv[], int argc);
static void	initialize_forks(t_table *data);
static void	ft_bzero(void *s, size_t n);

int	main(int argc, char *argv[])
{
	t_table	data;

	ft_bzero(&data, sizeof(data));
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
		handle_error(&data, ERR_INVALID_ARG, NULL);
		return (1);
	}
}

static void	initialize_table(t_table *data, char *argv[], int argc)
{
	int	i;

	i = 0;
	parse_args(argv, data, argc);
	data->dead_index = -1;
	data->philos = ft_calloc(sizeof(t_philo), data->philo_count);
	handle_error(data, ERR_MALLOC_FAIL, data->philos);
	while (i < data->philo_count)
	{
		data->philos[i].identity = i + 1;
		data->philos[i].data = data;
		data->philos[i].last_meal_time = get_time();
		i++;
	}
	if ((data->philo_count != data->philos[i - 1].identity))
		handle_error(data, ERR_THREAD_FAIL, NULL);
}

static void	init_mutex(t_table *data)
{
	handle_mutex_error(data, pthread_mutex_init(&data->death_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->start_flag_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->check_meal_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->print_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->must_meal_mutex, NULL));
	handle_mutex_error(data, pthread_mutex_init(&data->meal_mutex, NULL));
}

static void	initialize_forks(t_table *data)
{
	int	i;

	i = -1;
	init_mutex(data);
	data->forks = ft_calloc(data->philo_count, sizeof(pthread_mutex_t));
	handle_error(data, ERR_MALLOC_FAIL, data->forks);
	while (++i < data->philo_count)
	{
		handle_mutex_error(data, pthread_mutex_init(&data->forks[i], NULL));
		handle_mutex_error(data,
			pthread_mutex_init(&data->philos[i].eat_count_mutex, NULL));
	}
	i = -1;
	while (++i < data->philo_count)
	{
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->philo_count];
	}
}

static void	ft_bzero(void *s, size_t n)
{
	size_t	i;
	char	*str;

	i = 0;
	str = (char *)s;
	while (i < n)
	{
		str[i] = '\0';
		i++;
	}
}
