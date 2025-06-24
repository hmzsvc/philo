#include "../lib/philo.h"

int ft_atoi(char *str, int *res)
{
	int	i;
	int	sign;

	*res = 0;
	i = 0;
	sign = 1;
	while (*(str + i) == '+' || *(str + i) == '-')
	{
		if (*(str + i) == '-')
			sign *= -1;
		i++;
	}
	while (*(str + i) >= '0' && *(str + i) <= '9' || *(str + i) != '\0')
	{
		if (!(*(str + i) >= '0' && *(str + i) <= '9'))
			return (1);	// Buralara check error'a gönderip error message dönülmeli
		else
			*res = (*res * 10) + (*(str + i) - '0');
		i++;
	}
	*res *= sign;
	if (*res <= 0)
		return (1);	// Buralara check error'a gönderip error message dönülmeli
	return (0);
}
