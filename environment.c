#include "shell.h"

/**
 * _myenvironment - prints the current environment
 * @info: Structure containing potential arguments. Used to maintain
 * constant function prototype.
 * Return: Always 0
 */
int _myenvironment(info_t *info)
{
	print_list_string(info->env);
	return (0);
}

/**
 * _getenviron - gets the value of an environ variable
 * @info: Structure containing potential arguments. Used to maintain
 * @name: env var name
 * Return: the value 
 */
char *_getenviron(info_t *info, const char *name)
{
	list_t *node = info->env;
	char *p;

	while (node)
	{
		p = starts_with(node->str, name);
		if (p && *p)
			return (p);
		node = node->next;
	}
	return (NULL);
}

/**
 * _mysetenviron - Initialize a new environment variable, or modify an existing one
 * @info: Structure containing potential arguments. Used to maintain
 * constant function prototype.
 * Return: Always 0
 */
int _mysetenviron(info_t *info)
{
	if (info->argc != 3)
	{
		_eput("Incorrect number of arguement \n");
		return (1);
	}
	if (_setenviron(info, info->argv[1], info->argv[2]))
		return (0);
	return (1);
}

/**
 * _myunsetenviron - Remove an environment variable
 * @info: Structure containing potential arguments. Used to maintain
 * constant function prototype.
 * Return: Always 0
 */
int _myunsetenviron(info_t *info)
{
	int i;

	if (info->argc == 1)
	{
		_eput("Too few arguement \n");
		return (1);
	}
	for (i = 1; i <= info->argc; i++)
		_unsetenviron(info, info->argv[i]);

	return (0);
}

/**
 * populate_environ_liste - populates environ linked list
 * @info: Structure containing potential arguments. Used to maintain
 * constant function prototype.
 * Return: Always 0
 */
int populate_environ_liste(info_t *info)
{
	list_t *node = NULL;
	size_t i;

	for (i = 0; environ[i]; i++)
		add_node_end_ls(&node, environ[i], 0);
	info->env = node;
	return (0);
}

