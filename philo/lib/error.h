/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:55:27 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/02 20:55:28 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef ERROR_H
# define ERROR_H

typedef enum s_error_code
{
	ERR_MALLOC_FAIL = 1,
	ERR_INVALID_ARG = 2,
	ERR_THREAD_FAIL = 3,
	ERR_ATOI_FAIL = 4,
}	t_error_code;

#endif