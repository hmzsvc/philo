/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:54:41 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/07 11:35:32 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/philo.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	t_table data;

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
