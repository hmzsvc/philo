/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:55:37 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/05 22:41:57 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>

typedef struct s_table	t_table;

typedef struct s_philo
{
	int				identity;
	int				left_fork_bool;
	int				right_fork_bool;
	int				eat_count;
	long long		last_meal_time;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	eat_count_mutex;
	t_table			*data;
}	t_philo;

typedef struct s_table
{
	int				philo_count;
	int				dead_index;
	int				must_eat;
	int				is_dead;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				start_flag;
	int				must_meal_loop;
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
}	t_table;

// int			ft_atoi(char *str, int *res);
void		initialize_table(t_table *data, char *argv[], int argc);
void		initialize_forks(t_table *data);
void		create_philo(t_table *data);
void		handle_error(t_table *data, int err_code, void *ptr);
void		reset_table(t_table *data);
long long	get_time_in_ms(void);
void		setup_philosopher_monitor(t_table *data);
int			check_dead(t_philo *philo);
int			check_start_flag(t_philo *philo);
int			handle_dead(t_philo *philo);
void		philo_eat(t_philo *philo);
void		philo_sleep(t_philo *philo);
void		philo_thinking(t_philo *philo);
void		philo_dead(t_philo philo);
void		last_meal_added(t_philo *philo);
void		sync_philo_start(t_philo *philo);
void		parse_args(char *argv[], t_table *data, int argc);
int			check_meal_goal(t_philo *philo);
void		philo_take_fork(t_philo *philo);
void		print(t_philo *philo, char *str);
void		philo_join(t_table *data);
void		*ft_calloc(size_t count, size_t size);
void		handle_mutex_error(t_table *data, int value);
// int			check_and_handle_death(t_table *data, int philo_index);
void		set_time(t_table *data);
void		ft_usleep(int wait_time, t_philo *philo);
// void		wait_start(t_table *data);


#endif