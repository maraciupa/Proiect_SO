#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>


//PHASE 3//
/* We'll make a separate function to verify if the monitor is already running, so that it is easier
to modify this code from phase 2: */

int check_monitor()
{
	int fd=open(".monitor_pid", O_RDONLY);
	if(fd==-1)
	{
		return 0; //this means that there aren't any other monitors running in that moment
	}
	//checking the PID://
	char pids[50];
	int read_pid=read(fd, pids, sizeof(pids)-1);
	close(fd);
	if(read_pid<=0)
	{
		return 0; //the pid couldn't be read
	}
	int pid=atoi(pids); //char-->int
	if(pid<=0)
	{
		return 0; //this time, the pid is invalid
	}
	//checking if the process is already running:
	if((kill(pid, 0))==0)
	{
		return pid; //the monitor is already running
	}
	
	return 0;
}

//PHASE 2//
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
		
		char *message="SIGUSR1:Signal SIGUSR1; Sensing a new report!\n";
		write(1, message, strlen(message)); //1 for the standard output
		
	}
	else if(wanted_signal==SIGINT) 
	{
		//the program ends when it receives this signal: //
		unlink(".monitor_pid"); //deleting the hidden file created earlier
		//writing a message for the standard output after deleting: //
	
		char *message_delete="STOP:Signal SIGINT; The hidden file was deleted and the program ends!";
		write(1, message_delete, strlen(message_delete)); //1 for the standard output
		
		exit(0);
	}

}


int main()
{
	//PHASE 3//
	int verify_pid=check_monitor();
	if(verify_pid>0)
	{
		char message[150];
		sprintf(message, "ERROR:The monitor is already running! PID: %d", verify_pid);
		write(1, message, strlen(message));
		exit(-1); //because we treat this case as an error;
	}
	
	//PHASE 2//
	struct sigaction act={0};
	act.sa_handler=program_monitor_file;
	act.sa_flags=0; //making sure that the flags are set on 0

	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGINT, &act, NULL);

	create_hidden_file();
	char *message_started="SIGUSR1:Monitor started!\n";
	write(1, message_started, strlen(message_started));

  
	while(1)
	{
		pause();
	} 
}