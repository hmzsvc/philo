/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hasivaci <hasivaci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 17:46:22 by huozturk          #+#    #+#             */
/*   Updated: 2025/06/24 13:22:40 by hasivaci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

typedef	enum s_error_code
{
	ERR_MALLOC_FAIL = 1,
	ERR_INVALID_ARG = 2,
} t_error_code;

#endif