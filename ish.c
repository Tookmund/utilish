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
	switch (pid)
	{
		case 0:
			execvp(s,argv);
			break;
		default:
			waitpid(pid,NULL,0);
	}
}
int main (int argc, char** argv)
{
	char c;
	int bufsize = 100;
	int curbuf = 0;
	// TODO: Catch malloc errors
	char* buf = (char*)malloc(bufsize);
	printf(prompt());
	while (c != EOF)
	{
		c = getchar();
		switch(c)
		{
			case '\n':
				eval(buf);
				printf(prompt());
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
					// TODO: Catch realloc errors
					buf = realloc(buf,bufsize);
				}
		}
		//printf("buf: (%s) curbuf: %d\n",buf,curbuf);
	}
}
