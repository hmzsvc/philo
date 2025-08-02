/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:46:30 by huozturk          #+#    #+#             */
/*   Updated: 2025/08/02 14:50:35 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>

typedef struct s_data	t_data;

typedef struct s_philo
{
	int				id;
	int				left_fork_bool;
	int				right_fork_bool;
	int				eat_count;
	long long		last_meal_time;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	eat_count_mutex;
	t_data			*data;
}	t_philo;

typedef struct s_data
{
	int				philo_count;
	int				dead_index;
	int				must_eat;
	int				is_dead;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				start_flag;
	int				must_meal_num;
	long long		start_time;
	long long		last_meal_philo;
	pthread_t		monitor_philo;
	pthread_mutex_t	*forks;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	check_meal_mutex;
	pthread_mutex_t	start_flag_mutex;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	meal_mutex;
	pthread_mutex_t	must_meal_mutex;
	t_philo			*philos;
}	t_data;

int			ft_atoi(char *str, int *res);
void		init_philo(t_data *data, char *argv[], int argc);
void		init_forks(t_data *data);
void		create_philo(t_data *data);
void		error_check(t_data *data, int err_code, void *ptr);
void		cleanup(t_data *data);
long long	get_time_in_ms(void);
void		monitor_philo_create(t_data *data);
int			check_dead(t_philo *philo);
int			check_start_flag(t_philo *philo);
void		handle_dead(t_philo *philo);
void		philo_eat(t_philo *philo);
void		philo_sleep(t_philo *philo);
void		philo_thinking(t_philo *philo);
void		philo_dead(t_philo philo);
void		last_meal_added(t_philo *philo);
void		sync_philo_start(t_philo *philo);
void		parse_args(char *argv[], t_data *data, int argc);
void		check_meal_goal(t_philo *philo);
void		philo_take_fork(t_philo *philo);
void		print(t_philo *philo, char *str);
void		philo_join(t_data *data);
void		*ft_calloc(size_t count, size_t size);
void		error_check_mutex(t_data *data, int value);
void		check_and_handle_death(t_data *data, int philo_index);
void		set_time(t_data *data);
void		ft_usleep(int wait_time, t_philo *philo);
void		wait_start(t_data *data);


#endif