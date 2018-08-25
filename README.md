# LinuxShell
A simple Linux shell written in C. Very primitive shell that helps in understanding the concepts behind a Linux shell. 

After entering the shell, we can execute any binary files or scripts present in the system just like a normal shell. 

It also contain some builtins like 

* help - Display list of shell builtins
* cd - change working directory of the shell
* exit -  exit the shell

More builtins will be added shortly.

### Implementation

It contains three main components

* readline - Constantly wait for user action and read the user input
* parseline - Parses the input into commands and its arguments
* execute - Execute the process associated with the command and pass in the relevant arguments

![alt text](https://user-images.githubusercontent.com/7611872/44621703-7edae380-a8c8-11e8-84c0-122cb5865068.png)


#### How do we execute commands

Once an input is parsed, we separate the command and its arguments. Then we check if the command is a shell builtin or not. If it is a builtin function then we call the corresponding function in our code and the action is completed.

If the command is not a builtin function of the shell, we assume it is a binary or script file in the system. So to execute it we need to create a seperate process. Like standard unix shells, `fork` is used to create a new process and `exec` is used to change the context of the new process. 

The shell (which is now the parent process for the new process) waits for its child process to finish. The child process can exit either on its own (this exit does not mean the process completed successfully. ALways check the status to know the process exited successfully or  failed) or killed by a non maskable external signal. After this user can enter new commands and the same process continues.

