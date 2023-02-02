
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token NOTOKEN GREAT NEWLINE GREAT_GREAT PIPE AMPERSAND LOWER GREATAMPERSAND GREATGREATAMPERSAND TWOGREAT

%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
   pipe_list iomodifier_list background_optional NEWLINE { 
//printf("   Yacc: Execute command\n");
    Command::_currentCommand.execute();
  }
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);

	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);
	       
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;
	pipe_list:
	pipe_list PIPE command_and_args
	| command_and_args

iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._outCounter++;
	}
	| GREAT_GREAT WORD {
printf("   GREAT GREAT WORD: insert output \"%s\"\n", $2);
    	Command::_currentCommand._outFile = $2;
		Command::_currentCommand._append = 1;
		Command::_currentCommand._outCounter++;
   
		
	}
	| GREATAMPERSAND WORD {
printf("   Yacc: insert output \"%s\"\n", $2);
    	Command::_currentCommand._outFile = $2;
    	Command::_currentCommand._errFile = $2;
		Command::_currentCommand._outCounter++;
	}
	| GREATGREATAMPERSAND WORD {
printf("   Yacc: insert output \"%s\"\n", $2);
    	Command::_currentCommand._outFile = $2;
    	Command::_currentCommand._errFile = $2;
    	Command::_currentCommand._append = 1;
		Command::_currentCommand._outCounter++;
	}
	| LOWER WORD {
printf("   Yacc: insert input \"%s\"\n", $2);
    	Command::_currentCommand._inputFile = $2;
		Command::_currentCommand._inCounter++;
	}
	| TWOGREAT WORD {
printf("   Yacc: insert output \"%s\"\n", $2);
    	Command::_currentCommand._errFile = $2;
	}
	;
	iomodifier_list:
	iomodifier_list iomodifier_opt
	| iomodifier_opt
	| /* can be empty */
	;
	background_optional:
	AMPERSAND {
		Command::_currentCommand._background = 1;
	}
	| /* can be empty */
	;

%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
