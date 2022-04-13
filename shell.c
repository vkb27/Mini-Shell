#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*         Shell_Init         */

int main(int ac, char **av, char **env);
void prompt(void);
void handle(int signals);
void _EOF(char *buffer);
void shell_exit(char **command);

/*        create_child        */

void create_child(char **command, char *name, char **env, int cicles);
int change_dir(const char *path);

/*          Execute           */

void execute(char **command, char *name, char **env, int cicles);
void print_env(char **env);
char **_getPATH(char **env);
void msgerror(char *name, int cicles, char **command);

/*          Tokening           */

char **tokening(char *buffer, const char *s);

/*         Free Memory         */

void free_dp(char **command);
void free_exit(char **command);

/*      Auxiliar_Functions          */

int _strcmp(char *s1, char *s2);
unsigned int _strlen(char *s);
char *_strcpy(char *dest, char *src);
int _atoi(char *s);
char *_strcat(char *dest, char *src);

/*   _strcmp - A funtion that compares two strings   */
int _strcmp(char *s1, char *s2)
{
	unsigned int i = 0;

	while (s1[i])
	{
		if (s1[i] != s2[i])
			return (0);
		i++;
	}

	return (1);
}


/* _strlen - A function that finds returns The length of the string */
unsigned int _strlen(char *s)
{
	unsigned int len = 0;

	while (s[len])
		len++;

	return (len);
}

/*  _strcpy - A function that copies a string to another string; returns pointer to copied string */
char *_strcpy(char *dest, char *src)
{
	char *aux = dest;

	while (*src)
		*dest++ = *src++;
	*dest = '\0';
	return (aux);
}

/* _strcat - a function that concatenates two strings; returns pointer to the resulting string */
char *_strcat(char *dest, char *src)
{
	char *temp = dest;

	while (*dest)
		dest++;

	*dest++ = '/';
	while (*src)
		*dest++ = *src++;
	return (temp);
}

/*  _atoi - a function that converts string to integer; returns integer from conversion. */
int _atoi(char *s)
{
	int sign = 1;
	unsigned int total = 0;
	char null_flag = 0;

	if (s == NULL)
		return (0);
	while (*s)
	{
		if (*s == '-')
			sign *= -1;
		if (*s >= '0' && *s <= '9')
		{
			null_flag = 1;
			total = total * 10 + (*s - '0');
		}
		else if (*s < '0' || *s > '9')
		{
			if (null_flag == 1)
				break;
		}
		s++;
	}
	if (sign < 0)
		total = (-1 * (total));
	return (total);
}
/* create_child - A function that creates a sub process */
void create_child(char **command, char *name, char **env, int cicles)
{
	// command: The pointer to tokenized command
    // name: The pointer to the name of shell.
    // env: The pointer to the enviromental variables.
 	// cicles: Number of executed cicles.
	int pid = 0;
	int status = 0;
	int wait_error = 0;

	pid = fork();
	if (pid < 0)
	{
		perror("Error: ");
		free_exit(command);
	}
	else if (pid == 0)
	{
		execute(command, name, env, cicles);
		free_dp(command);
	}
	else
	{
		wait_error = waitpid(pid, &status, 0);
		if (wait_error < 0)
		{
			free_exit(command);
		}
		free_dp(command);
	}
}


/* change_dir - A function that changes working directory; 0 on success, -1 on failure. */
int change_dir(const char *path)
{
	// path: The new current working directory.
	char *buf = NULL;
	size_t size = 1024;

	if (path == NULL)
		path = getcwd(buf, size); // if cd command passed with no other arguments then path changes to home directory
	if (chdir(path) == -1) // checking if directory changed successfully
	{
		perror(path);
		return (98);
	}
	return (1);
}
/* execute - A function that executes a command */
void execute(char **command, char *name, char **env, int cicles)
{
	//command: The pointer to tokienized command
	//name: The name of the shell.
	//env: The pointer to the enviromental variables.
	//cicles: Number of executed cicles.

	char **pathways = NULL, *full_path = NULL;
	struct stat st;
	unsigned int i = 0;

	if (_strcmp(command[0], "env") != 0)
		print_env(env); // printing the current environment
	if (stat(command[0], &st) == 0) // system call to check info of command passed
	{
		if (execve(command[0], command, env) < 0) // command executed if it is with correct path
		{
			// checking if not executed successfully
			perror(name);
			free_exit(command);
		}
	}
	else
	{
		pathways = _getPATH(env);
		while (pathways[i])
		{
			full_path = _strcat(pathways[i], command[0]); 
			// concatenating path to get the correct location of the command to be properly executed
			i++;
			// system call for checking if proper path of command passed
			if (stat(full_path, &st) == 0) 
			{
				if (execve(full_path, command, env) < 0) // execution of command
				{
					// checking if not executed successfully
					perror(name);
					free_dp(pathways);
					free_exit(command);
				}
				return;
			}
		}
		msgerror(name, cicles, command);
		free_dp(pathways); // clearing up memory
	}
}


/* print_env - A small function that prints all enviromental variables */
void print_env(char **env)
{
	size_t i = 0, len = 0;

	while (env[i])
	{
		len = _strlen(env[i]);
		write(STDOUT_FILENO, env[i], len);
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}


/* _getPATH - A function to gets the full value from enviromental variable PATH  */
char **_getPATH(char **env)
{
	//env: The pointer to enviromental variables
	char *pathvalue = NULL, **pathways = NULL;
	unsigned int i = 0;

	pathvalue = strtok(env[i], "="); // tokenising env variables
	while (env[i])
	{
		if (_strcmp(pathvalue, "PATH"))
		{
			pathvalue = strtok(NULL, "\n");
			pathways = tokening(pathvalue, ":");
			return (pathways);
			// All tokenized pathways returned for commands
		}
		i++;
		pathvalue = strtok(env[i], "=");
	}
	return (NULL);
}


/* msgerror - A function that prints message not found */
void msgerror(char *name, int cicles, char **command)
{
	char c;
	//name: The name of the shell.
	//cicles: Number of cicles.
	//command: The pointer to tokenized command.
	c = cicles + '0';
	write(STDOUT_FILENO, name, _strlen(name));
	write(STDOUT_FILENO, ": ", 2);
	write(STDOUT_FILENO, &c, 1);
	write(STDOUT_FILENO, ": ", 2);
	write(STDOUT_FILENO, command[0], _strlen(command[0]));
	write(STDOUT_FILENO, ": not found\n", 12);
}


/* free_dp - A function that frees all the memory allocated for command to which the pointer is pointing */
void free_dp(char **command)
{
	size_t i = 0;

	if (command == NULL)
		return;

	while (command[i])
	{
		free(command[i]);
		i++;
	}

	if (command[i] == NULL)
		free(command[i]);
	free(command);
}


/* free_exit - A function that frees all the memory allocated at the pointer and exit */
void free_exit(char **command)
{
	size_t i = 0;

	if (command == NULL)
		return;

	while (command[i])
	{
		// freeing each pointer in a loop
		free(command[i]);
		i++;
	}

	if (command[i] == NULL)
		free(command[i]);
	free(command);
	exit(EXIT_FAILURE);

}

/* tokening - A function that split and create a full string command */

char **tokening(char *buffer, const char *s)
{
	// s: The delimiter for strtok.
	// buffer: The pointer to input string.

	// using the function token and storing all tokens in commands
	char *token = NULL, **commands = NULL;
	size_t bufsize = 0;
	int i = 0;

	if (buffer == NULL)
		return (NULL);

	bufsize = _strlen(buffer);
	commands = malloc((bufsize + 1) * sizeof(char *));
	if (commands == NULL)
	{
		perror("Unable to allocate buffer");
		free(buffer);
		free_dp(commands);
		exit(EXIT_FAILURE);
	}

	token = strtok(buffer, s);
	while (token != NULL)
	{
		commands[i] = malloc(_strlen(token) + 1);
		if (commands[i] == NULL)
		{
			perror("Unable to allocate buffer");
			free_dp(commands);
			return (NULL);
		}
		_strcpy(commands[i], token);
		token = strtok(NULL, s);
		i++;
	}
	commands[i] = NULL; // marking the end by having the last ptr as NULL
	return (commands);
}



// MAIN function which runs the shell
int main(int ac, char **av, char **env)
{
	char *buffer = NULL, **command = NULL , **command_pipe1 = NULL,**command_pipe2 = NULL;
	FILE *ptr = fopen("history.txt","w"); // to keep track of history of commands executed
	size_t buf_size = 0;
	size_t chars_readed = 0;
	int cicles = 0;
	(void)ac;

	while (1)
	{

		cicles++;
		prompt();

		signal(SIGINT, handle);
		chars_readed = getline(&buffer, &buf_size, stdin);
		int ch = '|';
		// storing the entered input into the current buffer using stdin
        fprintf(ptr,"%s\n",buffer);
		if (chars_readed == EOF)
			_EOF(buffer);
			// calling exiting function when End Of File is encountered in the input

		else if (*buffer == '\n')
			free(buffer);
			// signifies an empty command where user just hits enter,
            // while loop continued to get the next command
        else if(strchr(buffer, ch) != NULL) // for piping condition (only for single pipes)
        {
            command = tokening(buffer, "|");
            command[0][_strlen(command[0]) - 1] = '\0';
            command[1][_strlen(command[1]) - 1] = '\0';

            command_pipe1 = tokening(command[0], " \0");
            command_pipe2 = tokening(command[1], " \0");

            /* Pipe-ing execution of single level piping of two commands */
            free(buffer);
            int fd[2];
            pid_t childPid;
            if (pipe(fd) != 0){ // checking if pipe created
                perror("Failed to create pipe");
                exit(EXIT_FAILURE);
            }

            if ((childPid = fork()) == -1){
				// checking if successfully forked
                perror("Failed to fork");
                exit(EXIT_FAILURE);
            }
			// successfully creating two child processes

            if (childPid == 0)  // after successful forking
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                
                if (((execvp(command_pipe1[0], command_pipe1))) < 0) {
                    perror("Failed to exec command 1 ");
                	exit(EXIT_FAILURE);
                }
            }
            else
            {
                dup2(fd[0], 0);
                close(fd[0]);
                close(fd[1]);
                
                if (((execvp(command_pipe2[0], command_pipe2))) < 0) {
                    perror("Failed to exec command 2 ");
                	exit(EXIT_FAILURE);
                }
            }
        }
		else
		{
			//("%s",strchr(buffer, '|'));
			buffer[_strlen(buffer) - 1] = '\0';
			// adding \0 character at the end of argument entered to
			// transform it into valid command
			command = tokening(buffer, " \0");
			// breaking entered string into word components

			free(buffer);
			// freeing buffer as required input stored in command variable

            int fd0,fd1,i = 0,in=0,out=0;
            char input[64],output[64];

            // finds where '<' or '>' occurs and make that argv[i] = NULL , to ensure that command wont't read that

            int saved_stdout = dup(1);
            int saved_stdin = dup(0);

            while(command[i])
            {

                if(strcmp(command[i],"<")==0)
                {
					// checcking for input redirection
                    command[i]=NULL;
                    strcpy(input,command[i+1]);
                    in=2;
                }

                else if(strcmp(command[i],">")==0)
                {
					// checcking for input redirection
                    command[i]=NULL;
                    strcpy(output,command[i+1]);
                    out=2;
                }

                else if(strcmp(command[i],">>")==0)
                {
					// checcking for input redirection
                    command[i]=NULL;
                    strcpy(output,command[i+1]);
                    out=3;
                }
                i++;
            }

            //if '<' char was found in string inputted by user
            if(in)
            {	
				// fd0 is file-descriptor
                int fd0;
                if ((fd0 = open(input, O_RDONLY, 0)) < 0) {
                    perror("Couldn't open input file");
                    continue;
                }
                // dup2() copies content of fdo as input
                dup2(fd0, STDIN_FILENO);

                close(fd0);
            }

            //if '>' char was found in string inputted by user
            if (out == 2)
            {
                int fd1 ;
                if ((fd1 = creat(output , 0644)) < 0) {
                    perror("Couldn't open the output file");
                    continue;
                }

                dup2(fd1, STDOUT_FILENO); // dup2() adds content of output into fd1
                close(fd1);
            }
			//if '>>' char was found in string inputted by user
            else if (out == 3)
            {
                int fd2 ;
                if ((fd2 = open(output, O_WRONLY|O_APPEND|O_CREAT,0644)) < 0) {
                    perror("Couldn't open the output file");
                    continue;
                }

                dup2(fd2, STDOUT_FILENO); 
                close(fd2);
            }

			if (_strcmp(command[0], "exit") != 0)
				shell_exit(command);

			else if (_strcmp(command[0], "cd") != 0)
				change_dir(command[1]);

			else
				create_child(command, av[0], env, cicles);

            dup2(saved_stdout, 1);
            close(saved_stdout);
            dup2(saved_stdin, 0);
            close(saved_stdin);
		}

		fflush(stdin);
		// clearing the stream or buffer to make sure input for next command
		// is not affected
		buffer = NULL, buf_size = 0;
		// reseting buffer and buffer size before the loop runs again

	}

    fclose(ptr);
	if (chars_readed == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);



}


// prompt - A function that prints the prompt

void prompt(void)
{
	if (isatty(STDIN_FILENO))
    // checks if pointer points to terminal or not
		write(STDOUT_FILENO, "My_Shell>> ", 12);
    /* using the system call to avoid buffer abnormalities while 
	using printf as buffer is constantly changing */
}


// handle - A function to handle Ctr + C signal.

void handle(int signals)
{
	(void)signals;
	write(STDOUT_FILENO, "\nMy_Shell>> ", 12); // prompt to display after Ctrl + C is entered
}


// _EOF - A function that checks if buffer is EOF

void _EOF(char *buffer)
{
	if (buffer)
	{
		free(buffer); // freeing memory
		buffer = NULL;
	}

	if (isatty(STDIN_FILENO)) // isatty checks if pointer is pointing at terminal for correct output display
		write(STDOUT_FILENO, "\n", 1);
	free(buffer);
	exit(EXIT_SUCCESS);
}


/*  shell_exit - A function that exits the shell */
void shell_exit(char **command)
{
	int sta_tus = 0;
	// freeing the synamic memory allocated to command pointer
	//and the exiting showing the status if input input is in form exit 0 ,exit 1,etc...
	if (command[1] == NULL)
	{
		free_dp(command);
		exit(EXIT_SUCCESS);
	}
	sta_tus = _atoi(command[1]);
	free_dp(command); 
	exit(sta_tus);
}


