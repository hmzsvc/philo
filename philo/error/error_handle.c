#include "../lib/philo.h"
#include "../lib/error.h"
#include <stdlib.h>
#include <stdio.h>

void	cleanup(t_data *data)
{
	if (data->philos)
		free(data->philos);
	if (data->forks)
		free(data->forks);
}

void	error_check(t_data *data, int err_code, void *ptr)
{
	if (ptr)
		return ;
	if (err_code == ERR_MALLOC_FAIL)
		printf("ERR_MALLOC_FAIL\n");
	else if (err_code == ERR_INVALID_ARG)
		printf("ERR_INVALID_ARG\n");
	if (data)
		cleanup(data);
	
	exit(1);
}

