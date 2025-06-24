/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: huozturk <huozturk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:46:30 by huozturk          #+#    #+#             */
/*   Updated: 2025/06/19 16:50:25 by huozturk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct s_data t_data;

typedef struct s_philo
{
	int	id;
	pthread_t thread;
	long long last_meal;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	pthread_mutex_t meal_mutex;
	pthread_mutex_t dead_mutex;
	t_data	*data;
} t_philo;

typedef struct s_data
{
	int				philo_count;
	int				is_dead;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	//int				dead;
	long long		start_time;
	pthread_mutex_t	*forks;
	//pthread_mutex_t	print_mutext;
	t_philo			*philos;
	pthread_t		monitor_philo;


	int	start_flag;

}	t_data;

int ft_atoi(char *str, int *res);
void	init_philo(t_data *data, char *argv[]);
void	init_forks(t_data *data);
void	create_philo(t_data *data);
void	error_check(t_data *data, int err_code, void *ptr);
void	cleanup(t_data data);
long long	get_time_in_ms(void);
void	monitor_philo(t_data *data);


#endif