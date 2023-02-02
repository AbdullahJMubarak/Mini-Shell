
/*
 * AbdullahJMubarak: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "command.h"
extern char **environ; 
bool onError = false;

const char *ls = "ls";
SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append = 0;
	_inCounter = 0;
	_outCounter = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_inCounter = 0;
	_outCounter = 0;
	_append = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}
int Command::cd_dir(int i) {
	
	if(strcmp(_simpleCommands[i]->_arguments[0], "cd") == 0){
		int error;
		if(_simpleCommands[i]->_numberOfArguments == 1){
			error = chdir(getenv("HOME"));
		} else {
			error = chdir(_simpleCommands[i]->_arguments[1]);
		}

		if(error < 0){
			perror("cd");
		}

		clear();
		prompt();
		return 1;
	}
	
	return 0;
}


void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();
   
	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec
// 	int defaultin = dup( 0 );
// 	int defaultout = dup( 1 );
// 	int defaulterr = dup( 2 );

// // Create new process for "ls"
// int outfd ;
// if(_outFile){
// 	outfd = creat( _outFile, 0666 );
// 	dup2( outfd, 1 );
// 	close( outfd );

// }
// 	int pid = fork();
// 	if ( pid == -1 ) {
// 		perror( "ls: fork\n");
// 		exit( 2 );
// 	}

// 	if (pid == 0) {
// 		//Child
		
// 		// close file descriptors that are not needed
		
// 		close( defaultin );
// 		close( defaultout );
// 		close( defaulterr );

// 		// You can use execvp() instead if the arguments are stored in an array
// 	execvp(_simpleCommands[0]->_arguments[ 0 ], _simpleCommands[0]->_arguments);

// 		// exec() is not suppose to return, something went wrong
// 		perror( "ls: exec ls");
// 		exit( 2 );
// 	}
// dup2( defaultin, 0 );
// 	dup2( defaultout, 1 );
// 	dup2( defaulterr, 2 );

// 	close( outfd );
// 	close( defaultin );
// 	close( defaultout );
// 	close( defaulterr );
// waitpid( pid, 0, 0 );
if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

//
	if(strcmp(_simpleCommands[0]->_arguments[0],"exit") == 0){
		printf("Good bye!!\n");
		exit(1);
	}

	if (_inCounter > 1 || _outCounter > 1) {
		printf("Ambiguous output redirect.\n");
		clear();
		prompt();
		return;
	}
	int tin = dup(0);
	int tout = dup(1);
	int terr = dup(2);

	int fdin;
	int	fdout;
	int fderr;

	if(_inputFile){
		fdin = open(_inputFile, O_RDONLY);
	}
	else {
		fdin = dup(tin);
	}	

	if(_errFile){
		if(_append){
			fderr = open(_errFile, O_WRONLY | O_APPEND | O_CREAT, 0600);
		}
		else {
			fderr = open(_errFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
		}
	}
	else {
		fderr = dup(terr);
	}

	dup2(fderr,2);
	close(fderr);

	int pid;

	for(int i = 0; i < _numberOfSimpleCommands; i++){
		//built in
		if(cd_dir(i))	return;

		dup2(fdin,0);
		close(fdin);

		if(i == _numberOfSimpleCommands-1){
			if(_outFile){
				if(_append){
					fdout = open(_outFile, O_WRONLY | O_APPEND | O_CREAT, 0600);
				}
				else {
					fdout = open(_outFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
				}
			}
			else {
				fdout = dup(tout);
			}
		} else {
			int fdpipe[2];
			pipe(fdpipe);
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		}

		dup2(fdout,1);
		close(fdout);


		pid = fork();

		if (pid == -1){
			perror("fork\n");
			exit(2);
		}

		if(pid == 0){
			if(strcmp(_simpleCommands[i]->_arguments[0], "printenv") == 0){
				char ** env = environ;

				while(*env){
					printf("%s\n", *env);
					env++;
				}
			}

			if(strcmp(_simpleCommands[i]->_arguments[0], "source") == 0){
				FILE * fp = fopen(_simpleCommands[i]->_arguments[1], "r");
                char cmdline [1024];

				fgets(cmdline, 1023, fp);
				fclose(fp);

				int tin = dup(0);
				int tout = dup(1);

				int fdpipein[2];
				int fdpipeout[2];

				pipe(fdpipein);
				pipe(fdpipeout);

				write(fdpipein[1], cmdline, strlen(cmdline));
				write(fdpipein[1], "\n", 1);

				close(fdpipein[1]);

				dup2(fdpipein[0], 0);
				close(fdpipein[0]);
				dup2(fdpipeout[1], 1);
				close(fdpipeout[1]);

				int ret = fork();
				if (ret == 0) {
					execvp("/proc/self/exe", NULL);
					_exit(1);
				} else if (ret < 0) {
					perror("fork");
					exit(1);
				}

				dup2(tin, 0);
				dup2(tout, 1);
				close(tin);
				close(tout);

				char ch;
				char * buffer = (char *) malloc (100);
				int i = 0;
	
				// Read from the pipe the output of the subshell
				while (read(fdpipeout[0], &ch, 1)) {
		 			if (ch != '\n')
		   		 	buffer[i++] = ch;
				}

				buffer[i] = '\0';
				printf("%s\n",buffer);

				} 
				else {
					execvp(_simpleCommands[i]->_arguments[0],_simpleCommands[i]->_arguments);
				perror("execvp");
					_exit(1);
				}
			}
		} //for

	dup2(tin,0);
	dup2(tout,1);
	dup2(terr,2);
	close(tin);
	close(tout);
	close(terr);

	if(!_background){
		waitpid(pid,NULL,0);
	}
	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;
void ctrl_c(int);
void m_file(int);
int yyparse(void);
__sighandler_t old_handler; 
int 
main()
{  signal (SIGCHLD, m_file );  
	old_handler =signal (SIGINT, ctrl_c );
	Command::_currentCommand.prompt();
	yyparse();
	signal (SIGINT, old_handler );
	return 0;
}
void ctrl_c(int signum) {
signal (SIGINT, ctrl_c); // make sure that the signal will excute ctrl-c function
// signals are automatically reset

}  
void m_file(int sginum){
pid_t pid=getpid();
  FILE *fdout;
fdout = fopen("file.log","a+");
fprintf(fdout,"the child which PID= %d Terminated\n",getpid());
}
