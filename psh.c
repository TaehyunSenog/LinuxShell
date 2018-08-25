#include<stdio.h>
#include<string.h> 
#include<sys/wait.h>
#include <unistd.h>

#define CHAR_LENGTH 126
#define WORD_LENGTH 16
#define EXIT_FAILURE -1
#define PSH_DELIM " \t\r\n\a"	

int pshCd(char** args);
int pshExit(char** args);
int pshHelp(char** args);

//Function pointers for builtin functions
char *builtin_str[] = {"cd","exit","help"};

int (*builtin_func[]) (char**) = {pshCd,pshExit,pshHelp};

/**
We don't know the lnegth of input.So, initially we start with a buffer size and increase the buffer if needed.
We cant' use scanf because 'space' is the delimiter for scanf
*/
char* readLine(){

	int buffer_size = CHAR_LENGTH;
	int pos = 0;
	char* buffer = malloc(sizeof(char) * buffer_size);
	int c;

	//If err in allocating memory exit the program
	if(buffer == NULL){
		printf("Err in allocating memory \n");
		exit(EXIT_FAILURE);
	}

	while(1){

		//Read a char. Note that 'c' is an int rather than a char. 'c' should be an int to compare it with EOF
		c = getchar();

		//If we face a EOF or line break, append a null character and return the char*
		if(c == EOF || c == '\n'){
			buffer[pos] = '\0';
			return buffer;
		}else{
			buffer[pos++] = c;
		}

		//If the input exceeds the default buffer size
		if(pos >= buffer_size){

			buffer_size += CHAR_LENGTH;
			buffer = realloc(buffer, sizeof(char) * buffer_size);

			if(buffer == NULL){
				printf("Err in reallocating memory\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

/**
*/
char** parseLine(char* line){

	int no_of_words = WORD_LENGTH;
	int pos = 0;
	char** words_ptr = malloc(sizeof(char*) * no_of_words);
	char* word;

	//If err in allocating memory exit the program
	if(words_ptr == NULL){
		fprintf(stderr, "parseLine : Err in allocating memory\n" );
		exit(EXIT_FAILURE);
	}

	word = strtok(line,PSH_DELIM);

	while(word != NULL){

		words_ptr[pos++] = word;
		word = strtok(NULL,PSH_DELIM);


		if(pos >= no_of_words){

			no_of_words += WORD_LENGTH;

			words_ptr = realloc(words_ptr, sizeof(char*) * no_of_words);

			if(words_ptr == NULL){
				fprintf(stderr, "parseLine : Err in reallocating memory\n" );
				exit(EXIT_FAILURE);
			}
		}
	}

	words_ptr[pos] = NULL;
	return words_ptr;
}

int createProcess(char** args){

	int status;
	pid_t pid,wpid;
	pid = fork();

	//fork failed
	if(pid < 0){
		perror("Psh");
	}
	//Child process
	else if(pid == 0){

		/*Execute the program. The program name is the first word user entered. First word plus ll other words go as an argument to program
		Exec should never return as once it is called the program context changes.So, if it returns that means teh exec has failed. So,
		terminate the program*/
		if(execvp(args[0],args) == -1){
			perror("Psh");
		}
		exit(EXIT_FAILURE);
	}
	//Parent process
	else{
		do{
			/**A process can terminate in two ways
			1. Calling exit or main function returns from the process
			2. External signal killed the process */

			/**waitpid can return for status changes other than the termination of program. That means we need to check if waitpid 
			returned on actual process termination or something else*/
			wpid = waitpid(pid,&status,WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

/** Return the number of builtin functions in the shell */
int countOfBuiltinFuncs(){
	return sizeof(builtin_str)/sizeof(char*);
}

/**Displays list of builtin functions in PSH */
int pshHelp(char** args){

	int i=0;

	printf("Prakash SH aka PSH\n");
	printf("List of bsh builtin commands:\n\n");
	for(i=0;i<countOfBuiltinFuncs();i++){
		printf("  %s\n", builtin_str[i]);
	}
	printf("Use the man page for info on other process\n");

	return 1;
}


/**Changes the working directory of shell*/
int pshCd(char **args){

	if(args[1] == NULL){
		fprintf(stderr, "Psh: expected argument to \"cd\" \n" );
	}else{
		if(chdir(args[1]) != 0){
			perror("Psh");
		}
	}

	return 1;
}

/**Exit the shell */
int pshExit(char** args){
	return 0;
}

int execute(char **args){

	int i;

	//An empty command i.e., nothing is typed int the shell
	if(args[0] == NULL){
		return 1;
	}

	for(i=0;i<countOfBuiltinFuncs();i++){

		if(strcmp(args[0],builtin_str[i]) == 0){
			return (builtin_func[i])(args);
		}
	}
	return createProcess(args);
}

void pshLoop(){

	char* line;
	char** words;
	int status;

	int i=0;
	char* cur;

	do{
		printf("#");
		line = readLine();

		//Print the line to stderr
		fprintf(stderr, "Line read: %s\n", line);

		words = parseLine(line);

		//Print all the tokens to stderr
		cur =  words[i++];
		while(cur != NULL){
			fprintf(stderr, "%s\n", cur);
			cur = words[i++];
		}

		status = execute(words);

	}while(status);

}


int main(int argc, char const *argv[])
{
	pshLoop();
	return 0;
}