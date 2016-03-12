/* ish -- Simple shell for utilix
 * Copyright 2016 Jacob Adams <tookmund@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void prompt()
{
	// TODO: Actual prompt with PS1 and stuff
	printf("[$] ");
}

char** getargv(char* s, char* delim)
{
	char* arg = strtok(s,delim);
	if (arg == NULL)
	{
		perror("ish strtok argv");
		return NULL;
	}
	size_t argc = sizeof(char*)*2;
	int i = 1;
	char** argv = malloc(argc);
	if (argv == NULL)
	{
		perror("ish argv malloc");
		return NULL;
	}
	argv[0] = arg;
	while (arg != NULL)
	{
		arg = strtok(NULL,delim);
		if (arg == NULL) continue;
		argc += sizeof(char*);
		argv = realloc(argv,argc);
		if (argv == NULL)
		{
			perror("ish argv realloc");
			free(argv);
			return NULL;
		}
		argv[i] = arg;
		i++;
	}
	argv[i] = NULL;
	return argv;
}

int run (char* s,int in,int out)
{
	int r = 0;
	char** argv = getargv(s," ");
	// TODO Error handling
	if (argv == NULL) exit(1);
	int pid = fork();
	//printf("exec: %s argv[0]: %s\n",exec,argv[0]);
	switch (pid)
	{
		case 0:
			if (in != 0) dup2(0,in);
			if (out != 1) dup2(1,out);
			execvp(argv[0],argv);
			// Only returns if something went wrong
			perror("ish execvp");
			break;
		case -1:
			perror("ish fork");
		default:
			waitpid(pid,&r,0);
			free(argv);
	}
	return 0;
}

void eval(char* s, int pipes)
{
	if (!pipes) run(s,0,1);
	else
	{
		char** programs = getargv(s,"|");
		if (programs == NULL) exit(1);
		for (int i = 0; i <= pipes; i++)
		{
			
		}
	}
}

int main (int argc, char** argv)
{
	char c = 1;
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
		freopen(argv[1],"r",stdin);
		interactive = 0;
	}
	else
	{
		prompt();
	}
	int pipes = 0;
	while (c != EOF)
	{
		c = getchar();
		switch(c)
		{
			case '\n':
				if (interactive) prompt();
				eval(buf,pipes);
				// Wipe array when done
				memset(buf,0,bufsize);
				curbuf = 0;
				pipes = 0;
				break;
			case EOF:
				break;
			case '|':
				pipes++;
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
						perror("ish realloc");
						free(buf);
						return 1;
					}
				}
		}
	}
	free(buf);
}
