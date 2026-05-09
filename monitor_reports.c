#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>

// Helper function to create the hidden .monitor_pid file: //

void create_hidden_file()
{
	int fd=open(".monitor_pid", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fd==-1)
	{
		perror("Error! couldn't open the hidden file!");
		exit(-1);
	}
	
	//getting the pid using the getpid() function:
	int pid=getpid();
	char pids[50];
	sprintf(pids, "%d", pid);

	write(fd, pids, strlen(pids)); //storing the main process id
	close(fd);
}

//regarding signals:

void program_monitor_file(int wanted_signal)
{
	if(wanted_signal==SIGUSR1)
	{
		//writing the message to the standard output: //
		
		char *message="Signal SIGUSR1; Sensing a new report!\n";
		write(1, message, strlen(message)); //1 for the standard output
		
	}
	else if(wanted_signal==SIGINT) 
	{
		//the program ends when it receives this signal: //
		unlink(".monitor_pid"); //deleting the hidden file created earlier
		//writing a message for the standard output after deleting: //
	
		char *message_delete="Signal SIGINT; The hidden file was deleted and the program ends!";
		write(1, message_delete, strlen(message_delete)); //1 for the standard output
		
		exit(0);
	}

}


int main()
{
	struct sigaction act={0};
	act.sa_handler=program_monitor_file;
	act.sa_flags=0; //making sure that the flags are set on 0

	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGINT, &act, NULL);

	create_hidden_file();
	printf("Monitor started!\n"); //testing to see if the monitor program was started

  
	while(1)
	{
		pause();
	} 
}