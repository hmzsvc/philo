/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsyn <hsyn@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:46:22 by huozturk          #+#    #+#             */
/*   Updated: 2025/07/12 18:50:04 by hsyn             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

typedef	enum s_error_code
{
	ERR_MALLOC_FAIL = 1,
	ERR_INVALID_ARG = 2,
	ERR_THREAD_FAIL = 3,
	ERR_ATOI_FAIL = 4,
} t_error_code;

#endif