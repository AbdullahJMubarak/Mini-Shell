
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	

">" 	{
		return GREAT;
	}

">>" {
	return GREAT_GREAT;
}

"2>" {
	return TWOGREAT;
}

"|" {
	return PIPE;
}

"&" {
	return AMPERSAND;
}

"<" {
	return LOWER;
}

">&" {
	return GREATAMPERSAND;
}

">>&" {
	return GREATGREATAMPERSAND;
}

`[^\n`]*` {
	char * command = strdup(yytext+1);

	if(command[yyleng-2] == '`')
		command[yyleng-2] = '\0';
	
	int tmpin = dup(0);
	int tmpout = dup(1);

	int fdpipein[2];
	int fdpipeout[2];

	pipe(fdpipein);
	pipe(fdpipeout);

	write(fdpipein[1], command, strlen(command));
	write(fdpipein[1], "\n", 1);
	write(fdpipein[1], "exit", 4);
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

	dup2(tmpin, 0);
	dup2(tmpout, 1);
	close(tmpin);
	close(tmpout);

	char ch;
	char * buffer = (char *) malloc (4096);
	int i = 0;
	
	// Read from the pipe the output of the subshell
	while (read(fdpipeout[0], &ch, 1)) {
		if (ch == '\n') buffer[i++] = ' ';
		else buffer[i++] = ch;
	}
	buffer[i] = '\0';

	for (i = strlen(buffer); i >= 0; i--) {
		unput(buffer[i]);
	}
}

["][^\n\"]*["] { 
	/* quotes */
	yylval.string_val = strdup(yytext+1);

    if (yylval.string_val[yyleng-2] == '"')
		yylval.string_val[yyleng-2] = 0;
	return WORD;
} 

[^ \t\n|><&]*\\[^ \t\n]* {
	/* escaping */
//printf("yyval=%s\n",yylval.string_val);
	int i = 0;
	char * esc = (char *)malloc(200);
	char * temp = yytext;

	while(*temp){
		if (*temp == '\\'){
			if(*(temp+1) == '\\'){
				temp = temp + 2;
				esc[i++] = '\\';
			} else {
				esc[i++] = *(++temp);
			} 
		} else {
			esc[i++] = *temp;
		}
		temp++;
	}

	esc[i] = '\0';
	yylval.string_val = strdup(esc);
	return WORD;
}


[^ \t\n][^ \t\n]*  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}

. 		{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
