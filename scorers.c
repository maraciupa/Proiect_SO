#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<time.h>
#define NRMAX 1000


typedef struct
{
	char name[100];
	int total_score;
	
} Inspector_s; //using a struct so that we can easily keep track of every Inspector's score

typedef struct 
{
   int id;
   char inspector_name[50];
   float lat; 
   float longit;
   char issue_category[50];
   int severity_level;
   time_t timestamp;
   char description[150];

}Report;

int main(int argc, char **argv)
{
	if(argc<2)
	{
		perror("Error! insufficient number of arguments!");
		exit(-1);
	}
	 
	char *district=argv[1];
	int i=0; //the counter for the inspectors
	//making a list of all the scores for each inspector so that we can sum them later:
	Inspector_s v[NRMAX];

	//creating the filepath:
	char filepath[50];
	sprintf(filepath, "%s/reports.dat", district);
	
	int fd=open(filepath, O_RDONLY);
	if(fd==-1)
	{
		exit(0); //not an error
	}

	Report r;
	while(read(fd, &r, sizeof(Report))>0) //reading from every report available
	{
			int ok=-1;
			for(int j=0; j<i; j++)
			{
				if(strcmp(v[j].name, r.inspector_name)==0)
				{
					ok=j; //we've found the inspector
					break;
				}
			}
			if(ok>-1)
			{
				v[ok].total_score=v[ok].total_score+r.severity_level;
			}
			else if(ok==-1)
			{
				//we need to add the new inspector in the list:
				v[i].total_score=0; //initialising 
				strcpy(v[i].name, r.inspector_name);
				v[i].total_score=v[i].total_score+r.severity_level;
				i++;
			}
		
	}
	for(int j=0; j<i; j++)
	{
		printf("DISTRICT: %s\n", district);
		printf("Inspector Name: %s, Score: %d\n", v[j].name, v[j].total_score);
	}

	close(fd);

}