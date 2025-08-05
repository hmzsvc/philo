/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 21:06:05 by hasivaci          #+#    #+#             */
/*   Updated: 2025/08/05 21:06:13 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

typedef enum s_error_code
{
	ERR_MALLOC_FAIL,
	ERR_INVALID_ARG,
	ERR_THREAD_FAIL,
	ERR_ATOI_FAIL,
}	t_error_code;

#endif