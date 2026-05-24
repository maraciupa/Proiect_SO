#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>


void start_monitor()
{
	int pfd[2]; //2 file descriptors
	if(pipe(pfd)<0)
	{
		perror("Error! couldn't create the pipe!");
		exit(-1);
	}
	int pid=fork();
	if(pid<0)
	{
		perror("Error! fork() didn't work!");
		exit(-1);
	}
	if(pid==0) //regarding hub_mon
	{
		close(pfd[0]);
		int monitor_pid=fork();
		if(monitor_pid<0)
		{
			perror("Error! the fork() for the monitor didn't work!");
			exit(-1);
		}
		if(monitor_pid==0) //the child process created by hub_mon
		{
			dup2(pfd[1], 1); //1 for the stdout
 			close(pfd[1]); //closing the writing end
			execlp("./monitor_reports", "monitor_reports", NULL);
			perror("Error! execlp() didn't work!");
			exit(-1);
		}
		close(pfd[1]); //closing the writing end for hub_mon
		exit(0);
	}
	
	close(pfd[1]);
	printf("hub_mon started!\n"); //to see that the hub_mon started, to differentiate better
	//we need to read from the monitor using this pipe:
	char message_read[500];
	int r;
	while((r=read(pfd[0], message_read, sizeof(message_read)-1))>0)
	{
		message_read[r]='\0';
		if(strncmp(message_read, "ERROR:", 6)==0)
		{
			printf("ERROR from monitor: %s\n", message_read+6);
			printf("hub_mon: monitor ended!\n");
			break;
		}
		else if(strncmp(message_read, "STOP:", 5)==0)
		{
			printf("STOP from monitor: %s\n", message_read+5);
			printf("hub_mon: monitor ended!\n");
			break;
		}
		else if(strncmp(message_read, "SIGUSR1:", 8)==0)
		{
			printf("SIGUSR1 from monitor: %s\n", message_read+8);
				
		}
		else
		{
			printf("UNKNOWN: %s\n", message_read);
		}
	}
	close(pfd[0]);	
	
} 


//PHASE 3 EX 2//

void calculate_scores(char *districts)
{
	//we need to separate the districts with STRTOK:
	char *p=strtok(districts, " ");
	while(p)
	{
		int pfd[2]; //we'll create a pipe that will be reused at every district name
		if(pipe(pfd)<0)
		{
			perror("Error! Couldn't create pipe for calculate_scores()! ");
			exit(-1);
		}
		int pid=fork();	
		if(pid<0)
		{
			perror("Error! fork couldn't work for calculate_scores()! ");
			exit(-1);
		}
		if(pid==0) //the child process
		{
			close(pfd[0]); //closing the reading end of the pipe
			dup2(pfd[1], 1);
			close(pfd[1]);

			execl("./scorer", "scorer", p, NULL);
			perror("Error! execl() didn't work!");
			exit(-1);
		}	
		//the parent process:
		close(pfd[1]); //closing the writing end of the pipe
		
		char s[500]; //works as a buffer for reading
		int s_read;
		while((s_read=read(pfd[0], s, sizeof(s)-1))>0)
		{
			s[s_read]='\0';
			printf("%s", s);
		}
		close(pfd[0]); //closing the reading end of the pipe to prepare for the next district
		wait(NULL);
		p=strtok(NULL, " ");
	}
	printf("\n");
	
}

int main()
{
	char command[150];
	printf("city_hub started!\n");
	while(1)
	{
		printf("city_hub> ");
		if(fgets(command, sizeof(command), stdin)==NULL)
		{
			exit(0);
		}
		command[strcspn(command, "\n")]='\0';
		if(strcmp(command, "start_monitor")==0)
		{
			start_monitor();
		}
		else if(strcmp(command, "exit")==0)
		{
			printf("you chose the exit command!\n");
			exit(0);
		}
		//for the 2nd exercise from phase 3:
		else if(strncmp(command, "calculate_scores", strlen("calculate_scores"))==0)
		{
			char *district_argument=command+strlen("calculate_scores");
			while(*district_argument==' ')
			{
				district_argument++;
			}
			if(strlen(district_argument)==0)
			{
				printf("Error! not enough district arguments for calculate_scores()! ");
				//exit(-1);
			}
			else
			{
				calculate_scores(district_argument);
			}
		}
		else
		{
			printf("Error! invalid command!\n");
			
		}
	}
}