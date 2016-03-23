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
#include <signal.h>

#include "ish.h"

// Not sure why this is a thing but whatever
typedef void (*sighandler_t)(int);

void eval(char* s, int pipes)
{
	if (checkkeywords(s)) return;
	if (!pipes) run(s,0,1);
	else
	{
		char** programs = getargv(s,"|");
		if (programs == NULL) return;
		int* oldpipes = NULL;
		int* newpipes = NULL;
		for (int i = 0; i <= pipes; i++)
		{
			newpipes = malloc(sizeof(int)*2);
			if (newpipes == NULL)
			{
				perror("ish malloc newpipes");
				return;
			}
			int p = pipe(newpipes);
			if (p < 0)
			{
				perror("ish pipe");
				return;
			}
			if (programs[i+1] == NULL) newpipes[1] = 1;
			if (oldpipes == NULL) run(programs[i],0,newpipes[1]);
			else run(programs[i],oldpipes[0],newpipes[1]);
			free(oldpipes);
			oldpipes = newpipes;
		}
	}
	// Allow everything to finish up
	wait(NULL);
}

int main (int argc, char** argv)
{
	sighandler_t sig = signal(SIGINT,SIG_IGN);
	if (sig == SIG_ERR) perror("ish sigint");
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
				eval(buf,pipes);
				// Wipe array when done
				memset(buf,0,bufsize);
				curbuf = 0;
				pipes = 0;
				if (interactive) prompt();
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
