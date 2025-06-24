#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


void	*create_success(void *arg)
{
	int *i = (int *)arg;
	printf("YES\n");
	*i = 1;
	printf("i: %d\n", *i);
	return NULL;
}

int	main()
{
	pthread_t	thread;
	int			i;

	i = 0;
	pthread_create(&thread, NULL, create_success, (void *)&i);
	//pthread_join(thread, NULL);

	if (i != 1)
	{
		printf("YES\n");
		usleep(50000);
	}
	printf("i: %d\n", i);
	return (0);
}