#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shellmemory.h"
#include "shell.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int MAX_ARGS_SIZE = 7;

int badcommand()
{
	printf("%s\n", "Unknown Command");
	return 1;
}

int badcommand2()
{
	printf("%s\n", "Bad command: Too many tokens");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist()
{
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int help();
int quit();
int set(char *var, char *value);
int print(char *var);
int run(char *script);
int badcommandFileDoesNotExist();
int echo(char *var);
int my_touch(char *var);
int my_ls();
int my_mkdir(char *var);
int my_cd(char *var);

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size)
{
	int i;

	if (args_size < 1)
	{
		return badcommand();
	}

	// return if more than 7 args
	if (args_size > 7)
	{
		return badcommand2();
	}

	for (i = 0; i < args_size; i++)
	{ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help") == 0)
	{
		//help
		if (args_size != 1)
			return badcommand();
		return help();
	}
	else if (strcmp(command_args[0], "quit") == 0)
	{
		//quit
		if (args_size != 1)
			return badcommand();
		return quit();
	}
	else if (strcmp(command_args[0], "set") == 0)
	{
		//set
		if (args_size < 3)
			return badcommand();
		if (args_size > 7)
			return badcommand2();

		char *result = (char *)calloc(1, 1000);

		char space = ' ';

		for (int i = 2; i < args_size; i++)
		{
			strncat(result, command_args[i], 50);
			if (i < args_size - 1)
			{
				strncat(result, &space, 1);
			}
		}
		return set(command_args[1], result);
	}
	else if (strcmp(command_args[0], "print") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return print(command_args[1]);
	}
	else if (strcmp(command_args[0], "run") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return run(command_args[1]);
	}
	else if (strcmp(command_args[0], "echo") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return echo(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_touch") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return my_touch(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_ls") == 0)
	{
		if (args_size != 1)
			return badcommand();
		return my_ls();
	}
	else if (strcmp(command_args[0], "my_mkdir") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return my_mkdir(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_cd") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return my_cd(command_args[1]);
	}
	else
		return badcommand();
}

int help()
{

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n";
	printf("%s\n", help_string);
	return 0;
}

int quit()
{
	printf("%s\n", "Bye!");
	exit(0);
}

int set(char *var, char *value)
{
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;
}

int echo(char *var)
{
	if (var[0] == '$')
	{
		if (strcmp(mem_get_value(var + 1), "Variable does not exist") == 0)
		{
			printf("\n");
		}
		else
		{
			printf("%s\n", mem_get_value(var + 1));
		}
	}
	else
	{
		printf("%s\n", var);
	}
	return 0;
}

int print(char *var)
{
	printf("%s\n", mem_get_value(var));
	return 0;
}

int my_touch(char *var)
{
	FILE *file = fopen(var, "w+");
	fclose(file);
	return 0;
}

int my_cd(char *var)
{
	int status = chdir(var);
	if (status < 0)
	{
		printf("%s\n", "Bad command: my_cd");
	}
	return 0;
}

int my_mkdir(char *var)
{
	int num;
	if (var[0] == '$')
	{
		if (strcmp(mem_get_value(var + 1), "Variable does not exist") == 0)
		{
			printf("%s\n", "Bad command: my_mkdir");
		}
		else
		{
			num = mkdir(mem_get_value(var + 1), 0777);
		}
	}
	else
	{
		num = mkdir(var, 0777);
	}

	return 0;
}

int my_ls()
{
	DIR *directory = opendir(".");
	struct dirent *dir;

	if (directory)
	{
		char stringArray[100][100];
		int counter = 0;
		while ((dir = readdir(directory)) != NULL)
		{
			strcpy(stringArray[counter], dir->d_name);
			counter++;
		}
		closedir(directory);

		char s[100];
		int i, j;
		for (i = 0; i < counter; i++)
		{
			for (j = i + 1; j < counter; j++)
			{
				if (strcmp(stringArray[i], stringArray[j]) > 0)
				{
					strcpy(s, stringArray[i]);
					strcpy(stringArray[i], stringArray[j]);
					strcpy(stringArray[j], s);
				}
			}
		}

		for (i = 0; i < counter; i++)
		{
			if (strcmp(stringArray[i], "..") != 0 && strcmp(stringArray[i], ".") != 0)
			{
				printf("%s\n", stringArray[i]);
			}
		}
	} //code was influenced by tutorialspoint.com
	return 0;
}

int run(char *script)
{
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script, "rt"); // the program is in a file

	if (p == NULL)
	{
		return badcommandFileDoesNotExist();
	}

	fgets(line, 999, p);
	while (1)
	{
		errCode = parseInput(line); // which calls interpreter()
		memset(line, 0, sizeof(line));

		if (feof(p))
		{
			break;
		}
		fgets(line, 999, p);
	}

	fclose(p);

	return errCode;
}
