#include "shell.h"

/**
 * hsh - main shell loop
 * @info : the parameter & return info struct
 * @av : the argument vector from main()
 *
 * Return: 0 on success, 1 on error, or error code
 **/
int hsh(info_t *info, char **av)
{
	ssize_t r = 0;
	int builtin_ret = 0;

	while (r != -1 && builtin_ret != -2)
	{
		clear_infos(info);
		if (is_interactive(info))
			_puts("$ ");
		_eputcharacter(BUF_FLUSH);
		r = get_line(info);
		if (r != -1)
		{
			set_infos(info, av);
			builtin_ret = find_builtin(info);
			if (builtin_ret == -1)
				find_cmd(info);
		}
		else if (is_interactive(info))
			_putchar('\n');
		free_infos(info, 0);
	}
	write_historie(info);
	free_infos(info, 1);
	if (!is_interactive(info) && info->status)
		exit(info->status);
	if (builtin_ret == -2)
	{
		if (info->err_num == -1)
			exit(info->status);
		exit(info->err_num);
	}
	return (builtin_ret);
}

/**
 * find_builtin - finds a builtin command
 * @info: the parameter & return info struct
 *
 * Return: -1 if builtin not found,
 * 0 if builtin executed successfully,
 * 1 if builtin found but not successful,
 * 2 if builtin signals exit()
 */
int find_builtin(info_t *info)
{
	int i, built_in_ret = -1;
	builtin_table builtintbl[] = {
		{"exit", exit_shell},
		{"env", _myenvironment},
		{"help", my_help},
		{"history", history},
		{"setenv", _mysetenviron},
		{"unsetenv", _myunsetenviron},
		{"cd", my_cd},
		{"alias", mimics_alias},
		{NULL, NULL}
	};

	for (i = 0; builtintbl[i].type; i++)
		if (str_cmp(info->argv[0], builtintbl[i].type) == 0)
		{
			info->line_count++;
			built_in_ret = builtintbl[i].func(info);
			break;
		}
	return (built_in_ret);
}

/**
 * find_cmd - finds a command in PATH
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void find_cmd(info_t *info)
{
	char *path = NULL;
	int i, k;

	info->path = info->argv[0];
	if (info->linecount_flag == 1)
	{
		info->line_count++;
		info->linecount_flag = 0;
	}
	for (i = 0, k = 0; info->arg[i]; i++)
		if (!delimeter(info->arg[i], " \t\n"))
			k++;
	if (!k)
		return;

	path = find_path(info, _getenviron(info, "PATH="), info->argv[0]);
	if (path)
	{
		info->path = path;
		fork_cmd(info);
	}
	else
	{
		if ((is_interactive(info) || _getenviron(info, "PATH=")
					|| info->argv[0][0] == '/') && exct_cmd(info, info->argv[0]))
			fork_cmd(info);
		else if (*(info->arg) != '\n')
		{
			info->status = 127;
			err_print(info, "not found\n");
		}
	}
}

/**
 * fork_cmd - forks a an exec thread to run comande
 * @info: the parameter & return info structure
 * Return: void
 */
void fork_cmd(info_t *info)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid == -1)
	{
	
		perror("Error:");
		return;
	}
	if (child_pid == 0)
	{
		if (execve(info->path, info->argv, get_environ(info)) == -1)
		{
			free_infos(info, 1);
			if (errno == EACCES)
				exit(126);
			exit(1);
		}
	}
	else
	{
		wait(&(info->status));
		if (WIFEXITED(info->status))
		{
			info->status = WEXITSTATUS(info->status);
			if (info->status == 126)
			err_print(info, "Permission denied\n");
		}
	}
}

