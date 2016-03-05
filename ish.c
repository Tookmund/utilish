#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "ish.h"

char* prompt()
{
	// TODO: Actual prompt with PS1 and stuff
	return "[$] ";
}

void eval(char* s)
{
	// TODO: Allow arguements
	printf("buf (%s)\n",s);
	char* argv[2];
	argv[0] = s;
	argv[1] = NULL;
	run(s,argv);
	// Wipe array when done
	memset(s,0,sizeof(s));
}

void run (char* s, char** argv)
{
	int pid = fork();
	int r = 0;
	switch (pid)
	{
		case 0:
			r = execvp(s,argv);
			if (r < 0) perror("execvp");
			break;
		case -1:
			perror("fork");
		default:
			waitpid(pid,NULL,0);
	}
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
	printf(prompt());
	while (c != EOF)
	{
		c = getchar();
		switch(c)
		{
			case '\n':
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
