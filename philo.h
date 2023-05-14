/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tliangso <tliangso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 23:31:56 by tliangso          #+#    #+#             */
/*   Updated: 2023/05/14 02:00:29 by tliangso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <sys/time.h>
# include <ctype.h>

# include "color.h"

typedef int				t_index;
typedef struct timeval	t_time;

typedef struct s_data
{
	int	num_philo;
	int	timedie;
	int	timeeat;
	int	timesleep;
	int	num_eat;
}	t_data;

typedef struct s_philo
{
	int			id;
	pthread_t	thread;
	long		starttime;
	long		lastmealtime;
	t_index		myfork;
	t_index		notmyfork;
	int			eat_count;
}	t_philo;

typedef struct s_env
{
	int				current_id;
	bool			is_dead;
	t_data			data;
	t_philo			*philo;
	pthread_mutex_t	*forks;
}	t_env;

# define PHILO		env->philo
# define DATA		env->data
# define TDIE		DATA.timedie
# define TEAT		DATA.timeeat
# define TSLP		DATA.timesleep
# define EAT_CNT	DATA.num_eat

# define FORK(id)	env->forks[id]

# define THINK " is thinking"
# define THK "%s%ld %d is thinking%s\n"
# define EAT "%s%ld %d is eating%s\n"
# define SLP "%s%ld %d is sleeping%s\n"
# define FRK "%s%ld %d has taken a fork%s\n"
# define DIE "%s%ld %d has die%s\n"

# define TDN(t) timediffnow(t)
# define PTHK(id) printf(THK, GRN, TDN(PHILO[id].starttime), id, reset)
# define PEAT(id) printf(EAT, BLU, TDN(PHILO[id].starttime), id, reset)
# define PSLP(id) printf(SLP, WHT, TDN(PHILO[id].starttime), id, reset)
# define PMYF(id) printf(FRK, YEL, TDN(PHILO[id].starttime), id, reset)
# define PNMYF(id) printf(FRK, MAG, TDN(PHILO[id].starttime), id, reset)
# define PDIE(id) printf(DIE, RED, TDN(PHILO[id].starttime), id, reset)

# define LOCKMYFORK(id) pthread_mutex_lock(&FORK(PHILO[id].myfork))
# define LOCKYOURFORK(id) pthread_mutex_lock(&FORK(PHILO[id].notmyfork))
# define UNLOCKMYFORK(id) pthread_mutex_unlock(&FORK(PHILO[id].myfork))
# define UNLOCKYOURFORK(id) pthread_mutex_unlock(&FORK(PHILO[id].notmyfork))
#endif