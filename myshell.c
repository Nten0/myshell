#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


/******  Function Prototypes  *****/


/******  Main  *****/
int main(int argc, char **argv)
{
	/***Pointers initialization***/
	char *input = NULL;
	char *buffer = NULL;
	char **token = NULL;
	char ***tokens = NULL;

	size_t bufSize = 0;
	int maxComLength = 0;
	pid_t child;
	pid_t pid;
	int status;

	while(1)
	{
		/***Console***/
		putchar('$');
		getline(&input, &bufSize, stdin);

		token = (char **)malloc(sizeof(char*));


		/***Input tokenization***/
		buffer = strtok(input, "|\n");
		int commands = 0;
		while(buffer != NULL)
		{
			token = (char **)realloc(token, (commands + 1) * sizeof(char*));
			token[commands] = buffer;
			if(strlen((char *)token[commands]) > maxComLength)
				maxComLength = strlen((char *)token[commands]);
			commands++;
			buffer = strtok(NULL, "|\n");
		}
		token = (char **)realloc(token, (commands + 1) * sizeof(char*));
		token[commands] = NULL;


		if (commands == 0){}
		else
		{
			/***3D array consists of every input command***/
			tokens = (char***)malloc(commands * sizeof(char**));
			for(int i = 0; i < commands; i++)
				tokens[i] = (char**)malloc((maxComLength + 1) * sizeof(char*));


			/***Command tokenization***/
			int parameters = 0;
			for(int i = 0; i < commands; i++)
			{
				parameters = 0;
				buffer = strtok(token[i]," \t");
				while(buffer != NULL )
				{
					tokens[i][parameters] = buffer;
					parameters++;
					buffer = strtok(NULL," \t");
				}
				tokens[i][parameters] = NULL;
			}

			/***Only one command***/
			if(commands == 1)
			{
				if (tokens[0][0] == NULL){}
				else if (strcmp(tokens[0][0], "exit") == 0)
					{exit(0);}
				else if (strcmp(tokens[0][0], "cd") == 0)
				{
					char *p = NULL;
					p = getcwd(p, bufSize);
					p = strcat(p, "/");
					p = strcat(p, tokens[0][1]);
					if(chdir(p) != 0)
						if(chdir(tokens[0][1]) !=0 ){}
				}
				else
				{
					child = fork();
					if(child < 0)
						exit(-1);
					else if (child == 0)
						execvp(tokens[0][0], tokens[0]);
					else
					{
						status = 0;
						waitpid(-1, &status, 0);
					}
				}
			}

			/***Pipes***/
			else
			{
				int NumOfPipes = commands - 1;
				int pipes[2*NumOfPipes];

				for(int i = 0; i < NumOfPipes; i++)
					pipe(pipes + i*2);

				for(int i=0; i < commands; i++)
				{
					child = fork();
					if(child < 0)
						exit(-1);
					else if(child == 0)
					{
						if( i != 0 )
							dup2(pipes[2*(i-1)], 0);

						if( i != NumOfPipes)
							dup2(pipes[2*i+1], 1);

						for(int j = 0; j < 2*NumOfPipes; j++)
							close(pipes[j]);

						execvp(tokens[i][0], tokens[i]);
					}
				}
				if(child > 0)
				{
					for(int i = 0; i < 2*NumOfPipes; i++)
						close(pipes[i]);
				}
				status = 0;
				while ((pid = wait(&status)) > 0);
			}
		}
	}

	/***Deallocation of memory***/
	free(input);
	free(buffer);
	free(token);
	free(tokens);

	return 0;
}

/******  Function Definitions  *****/

