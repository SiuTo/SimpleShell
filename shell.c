#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pwd.h>

#define MAX_LINE 80
#define MAX_LEN 50
#define MAX_COMMAND 100

void handler(int num)
{
	waitpid(-1, NULL, WNOHANG);
}

void print_prompt()
{
	char hostname[MAX_LEN], cwd[MAX_LEN];
	gethostname(hostname, MAX_LEN);
	struct passwd *pw = getpwuid(getuid());
	getcwd(cwd, MAX_LEN);
	printf("%s@%s:%s >> ", pw->pw_name, hostname, cwd);
}

int splitCommand(char *command, char **args)
{
	int k = 1;
	args[0] = strtok(command, " \t");
	while (1)
	{
		args[k] = strtok(NULL, " \t");
		if (args[k]==NULL) break;
		++k;
	}
	if (strcmp(args[k-1], "&")==0)
	{
		args[k-1] = NULL;
		return 1;
	}
	else return 0;
}

int main(void)
{
	char command[MAX_LINE];
	char *args[MAX_LINE/2+1];
	int should_run = 1;
	int top = 0;
	char stack[MAX_COMMAND][MAX_LINE];

	signal(SIGCHLD, handler);

	while (should_run)
	{
		print_prompt();
		fflush(stdout);
		if (fgets(command, MAX_LINE, stdin)==NULL)
		{
			perror("fgets");
			return 1;
		}
		command[strlen(command)-1] = '\0';
		if (strlen(command)==0) continue;
		else if (strcmp(command, "exit")==0) should_run = 0;
		else if (strcmp(command, "history")==0)
		{
			int i;
			for (i=0; i<10 && i<top; ++i)
				printf("\t%d %s\n", top-i, stack[top-i]);
		}
		else{
			if (strcmp(command, "!!")==0)
			{
				if (top) strcpy(command, stack[top]);
				else{
					printf("No commands in history.\n");
					continue;
				}
			}
			else if (command[0]=='!')
			{
				int index = atoi(command+1);
				if (0<index && index<=top) strcpy(command, stack[index]);
				else{
					printf("No such command in history.\n");
					continue;
				}
			}
			strcpy(stack[++top], command);
			int background = splitCommand(command, args);
			if (strcmp(args[0], "cd")==0)
			{
				int ret = chdir(args[1]);
				if (ret==-1) perror("cd");
				continue;
			}
			pid_t pid = fork();
			if (!pid)
			{
				int p1 = 0, p2 = 0, p3 = 0, i = 0;
				while (args[++i]!=NULL)
					if (strcmp(args[i], "<")==0) p1 = i;
					else if (strcmp(args[i], ">")==0) p2 = i;
					else if (strcmp(args[i], ">>")==0) p3 = i;

				if (p1) freopen(args[p1+1], "r", stdin);
				if (p2) freopen(args[p2+1], "w", stdout);
				if (p3) freopen(args[p3+1], "a", stdout);
				if (p1) args[p1] = NULL;
				if (p2) args[p2] = NULL;
				if (p3) args[p3] = NULL;

				int ret = execvp(args[0], args);
				if (ret==-1)
				{
					perror("execvp");
					return 1;
				}
			}
			if (!background) wait(NULL);
		}
	}
	
	return 0;
}

