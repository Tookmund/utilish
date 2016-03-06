#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "ish.h"

void prompt()
{
	// TODO: Actual prompt with PS1 and stuff
	printf("[$] ");
}

void eval(char* s)
{
	run(s);
	// Wipe array when done
	memset(s,0,sizeof(s));
}

int run (char* s)
{
	int pid = fork();
	int r = 0;
	char* exec = getexec(s);
	char** argv = getargv(s);
	//printf("exec: %s argv[0]: %s\n",exec,argv[0]);
	switch (pid)
	{
		case 0:
			execvp(exec,argv);
			// Only returns if something went wrong
			perror("execvp");
			break;
		case -1:
			perror("fork");
		default:
			waitpid(pid,&r,0);
	}
	return 0;
}
char** getargv(char* s)
{
	char** argv = (char**)malloc(2*sizeof(char*));
	if (argv == NULL)
	{
		perror("argvm");
		exit(1);
	}
	char* tok = strtok(s," \0");
	argv[0] = tok;
	// Start i at 3 to ensure there is enough space
	int j = 0;
	for(int i = 3; tok != NULL; i++)
	{
		// Actual index
		j = i-2;
		tok = strtok(NULL," \0");
		if (tok != NULL)
		{
			printf("%s\n",tok);
			argv = realloc(argv,i*sizeof(char*));
			if (argv == NULL)
			{
				perror("argvr");
				exit(1);
			}
			argv[j] = tok;
		}
		else
		{
			// Setup array for execvp
			argv[j] = NULL;
		}
	}
	return argv;
}
char* getexec(char* s)
{
	return strtok(s," ");
}
int main (int argc, char** argv)
{
	char c;
	int bufsize = 100;
	int curbuf = 0;
	char* buf = (char*)malloc(bufsize);
	if (buf == NULL)
	{
		perror("malloc");
		return 1;
	}
	memset(buf,0,bufsize);
	int interactive = 1;
	if (argc > 1)
	{
		// Passed a script
		// Run non-interactive
		FILE* r = freopen(argv[1],"r",stdin);
		interactive = 0;
	}
	else
	{
		prompt();
	}
	while (c != EOF)
	{
		c = getchar();
		switch(c)
		{
			case '\n':
				if (interactive) prompt();
				eval(buf);
				curbuf = 0;
				break;
			case EOF:
				break;
			default:
				if (curbuf < bufsize)
				{
					buf[curbuf] = c;
					curbuf++;
				}
				else
				{
					bufsize = bufsize*2;
					buf = realloc(buf,bufsize);
					if (buf == NULL)
					{
						perror("realloc");
						return 1;
					}
				}
		}
	}
	free(buf);
}
