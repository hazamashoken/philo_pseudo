#include "philo.h"

// number_of_philosophers 
// time_to_die 
// time_to_eat 
// time_to_sleep 
// [number_of_times_each_philosopher_must_eat]

void	free_env(t_env *env)
{
	int	i;

	i = 0;
	// free forks if got forks
	if (env->forks)
		free(env->forks);
	
	// free all thread in all philo
	if (env->philo)
	{	
		while (i < env->data.num_philo && PHILO[i].thread)
		{
			free(PHILO[i].thread);
			i++;
		}
		// then free philo
		free(env->philo);
	}
}

int	philo_error(t_env *env)
{
	if (env)
		free_env(env);
	write(STDERR_FILENO, "Error\n", 7);
	return (EXIT_FAILURE);
}

int	is_notalldigit(int ac, char **av)
{
	int		i;
	char	*tocheck;

	i = 1;
	while (i < ac)
	{
		tocheck = av[i];
		while (*tocheck)
			if (!isdigit(*tocheck++))
				return (EXIT_FAILURE);
		++i;
	}
	return (EXIT_SUCCESS);
}

int	build_env(int ac, char **av, t_env *env)
{
	env->forks = NULL;
	env->philo = NULL;
	env->is_dead = 0;
	if (is_notalldigit(ac, av))
		return (EXIT_FAILURE);
	env->data.num_philo = atoi(av[1]);
	env->data.timedie = atoi(av[2]);
	env->data.timeeat = atoi(av[3]);
	env->data.timesleep = atoi(av[4]);
	if (ac == 6)
		env->data.num_eat = atoi(av[5]);
	else
		env->data.num_eat = -1;
	return (EXIT_SUCCESS);
}

int	build_fork(t_env *env)
{
	int	i;

	i = 0;
	env->forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * env->data.num_philo);
	if (!env->forks)
		return (EXIT_FAILURE);
	env->philo = (t_philo *)malloc(sizeof(t_philo) * env->data.num_philo);
	if (!env->philo)
		return (free(env->forks), EXIT_FAILURE);
	while (i < env->data.num_philo)
	{
		PHILO[i].id = i + 1;
		PHILO[i].myfork = i;
		PHILO[i].notmyfork = (i + 1) % env->data.num_philo;
		PHILO[i].thread = NULL;
		pthread_mutex_init(&env->forks[i], NULL);
		PHILO[i].eat_count = 0;
		++i;
	}
	return (EXIT_SUCCESS);
}

long	gettimenow()
{
	t_time	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

long	timediffnow(long past)
{
	t_time	time;
	long	res;

	gettimeofday(&time, NULL);
	res = (time.tv_sec * 1000 + time.tv_usec / 1000) - past;
	return (res);
}

int	waitsiwa(long time, t_env *env)
{
	long	then;

	then = gettimenow();
	while (timediffnow(then) < time && !env->is_dead)
		usleep(200);
	return (env->is_dead);
}

void	*routine(void *arg)
{
	t_env	*env;
	int		id;

	env = (t_env *)arg;
	id = env->current_id;
	PHILO[id].id = id;

	PHILO[id].lastmealtime = PHILO[id].starttime;
	while (!env->is_dead && (PHILO[id].eat_count < EAT_CNT || EAT_CNT == -1))
	{
		PTHK(id);
		if (env->is_dead || LOCKMYFORK(id))
			return (NULL);
		PMYF(id);
		if (env->is_dead || LOCKYOURFORK(id))
			return (NULL);
		PNMYF(id);
		PEAT(id);
		PHILO[id].lastmealtime = gettimenow();
		if (waitsiwa(TEAT, env))
			return (NULL);
		if (env->is_dead || UNLOCKMYFORK(id))
			return (NULL);
		if (env->is_dead || UNLOCKYOURFORK(id))
			return (NULL);
		++PHILO[id].eat_count;
		PSLP(id);
		if (waitsiwa(TSLP, env))
			return (NULL);
	}
	if (!(PHILO[id].eat_count < EAT_CNT) && !env->is_dead)
	{
		env->is_dead = 1;
		PDIE(id);
	}
	return (NULL);
}

int	release_philo(t_env *env)
{
	int	i;

	i = 0;
	while (i <= env->data.num_philo)
	{
		printf("===%d==%ld===\n", i, PHILO[i].starttime);
		PHILO[i].starttime = gettimenow();
		env->current_id = i;
		pthread_create(&PHILO[i].thread, NULL, routine, env);
		usleep(10);
		i += 2;
		if (i >= env->data.num_philo && i % 2 == 0)
			i = 1;
	}
	return (EXIT_SUCCESS);
}

void	checkwho_die(t_env *env)
{
	int	id;

	id = 0;
	while (!env->is_dead)
	{
		if (id >= DATA.num_philo)
			id = 0;
		if (timediffnow(PHILO[id].lastmealtime) > TDIE)
			env->is_dead = 1;
		++id;
	}
}

void	check_die(t_env *env)
{
	int	i;

	i = 0;
	// end main relation with thread (don't wait for thread)
	while (i < env->data.num_philo)
	{
		pthread_detach(PHILO[i].thread);
		++i;
	}

	// infinite loop til somedie
	checkwho_die(env);

	// destory all mutex;
	while (i < env->data.num_philo)
	{
		pthread_mutex_destroy(&env->forks[i]);
		i++;
	}
}

void	cleanup_philo(t_env *env)
{
	check_die(env);

	// free all *alloc
	free(env->forks);
	free(env->philo);
}

int	main(int ac, char **av)
{
	t_env	env;

	// check args
	printf("check arg\n");
	if (ac != 5 && ac != 6)
		return (philo_error(NULL));

	// build env from ac av
	printf("build env\n");
	if (build_env(ac, av, &env))
		return (philo_error(NULL));

	// build mutex for forks
	printf("build forks\n");
	if (build_fork(&env))
		return (philo_error(&env));
	
	//create thread
	printf("create thread\n");
	if (release_philo(&env))
		return (philo_error(&env));
	
	//free all *alloc
	printf("clean up\n");
	cleanup_philo(&env);
	return (EXIT_SUCCESS);
}