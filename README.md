# Mini-Shell

Description :
 
 BASIC LEVEL IMPLEMENTION OF SHELL IN C PROGRAMMING LANGUAGE

 ***********************************************************************

 Input accepted after the prompt "My_Shell>> "
 User Inputted command executed in a separate process called child process.

 A command line always ends with a new line ( * ENTER * )
 When the user enters the keyword "exit",shell will end and returns the status 0
 
 Shell can't be executed after entering * Ctrl + C *
 
 The program prints the current enviroment when the user types *env*.
 
 This program executes the most common shell commands as 
 *ls*, *grep*, *find*, *pwd*, *rm*, *cp*, *mv*, *exit*,etc... with arguments.
 
 *************************************************************************
 Functionalities : 
 
     1. environment variables (*env*) 
     2. cd (all variations)
     3. history : executed by accessing the history.txt file
       i.e. cat history.txt -> displays you all previously entered commands
     4. exit
     5. man ( for manual )
     6. pwd (current directory)
     7. date, cal ( current date and calender respectively)
     8. ls
     9. rm
     10. mv
     11. grep
     12. find
     13. sleep
     14. more, less
     15. mkdir, rmdir
     16. chmod

 ( majorly all external commands can be executed unless it requires
  special host access )
  
 Provision for all kinds of redirection possible, input(<), output(>) and append(>>)
        *** BUT NOT WHEN PIPING IS ALSO USED FOR REDIRECTION ***

 Provision for piping (* single level only *), in process creating two-child processes and 
 executing commands separated by pipe ( | )
 
 If an executable cannot be found, shell prints an error message 
 and displays the prompt again.

 After every execution of command, memory allocated is freed by * free() * command

 *************************************************************************

 Description of how shell works:
    
    1. First, the parent process is created when the user run the program.
    
    2. Then, the *isatty()* function using *STDIN_FILENO* file descriptor -fd- to tests 
        if there is an open file descriptor referring to a terminal. 
        
        If *isatty()* returns 1, the prompt is showed using *write()* with *STDOUT_FILENO* as fd 
        and waits for an input user command line.
    
    3. When the user types a command, *getline()* function reads an entire line from the stream and *strtok()* 
       function breaks the inputted command into a sequence of non-empty tokens.
    
    4. Next, it creates a separate child process suing *fork()* that performs the inputted command. 
       Unless otherwise is specified, the parent process waits for the child to exit before continuing.
    
    5. After tokening the command, *execve()* function brings and executes it, 
       it frees all allocated memory with *free()*.
    
    6. Finally, the program returns to main process: prints the prompt, and waits for another user input.

    7. If shell encounters a error allocating file or operation on it or accessing it,
        shell may exit with respective success or failure values
        Kindly execute the shell program again if encountered with an error
        related to files.
