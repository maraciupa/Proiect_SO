#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>

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

//Creating files and using chmod(): //

void create_district_directory(char *district_name)
{
	//District directory --> 750:
	if(mkdir(district_name, 0750)==0)
	  {
		chmod(district_name, 0750);
	  }
}


//Permission bits//

void permission_verify(char *file, char *role, char action)
{
   struct stat st;
   if(lstat(file, &st)==-1)
    {
	return;
    }

   int has_access=0;
   
   if(strcmp(role, "manager")==0)
	{
	   if(action=='r' && (st.st_mode & S_IRUSR))
		{
			has_access=1;
		}
	   if(action=='w' && (st.st_mode & S_IWUSR))
		{
			has_access=1;
		}

	}
   else if(strcmp(role, "inspector")==0)
	{
	   if(action=='r' && (st.st_mode & S_IRGRP))
		{
			has_access=1;
		}
	   if(action=='w' && (st.st_mode & S_IWGRP))
		{
			has_access=1;
		}
	}
   else 
	{
		has_access=0;
	}


   if(has_access==0)
	{
	 	perror("Error! Permission denied!");
		exit(-1);
	}

}


void print_permission(mode_t mode)
{
	if(S_ISDIR(mode))
		{
			printf("d");
		}
	else
		{
			printf("-");
		}
	if(mode & S_IRUSR)
		{
			printf("r");
		}
	else
		{
			printf("-");
		}
	if(mode & S_IWUSR)
		{
			printf("w");
		}
	else
		{
			printf("-");
		}
	if(mode & S_IXUSR)
		{
			printf("x");
		}
	else
		{
			printf("-");
		}
	if(mode & S_IRGRP)
		{
			printf("r");
		}
	else
		{
			printf("-");
		}
	if(mode & S_IWGRP)
		{
			printf("w");
		}
	else
		{
			printf("-");
		}
	if(mode & S_IROTH)
		{
			printf("r");
		}
	else
		{
			printf("-");
		}
	if(mode & S_IWOTH)
		{
			printf("w");
		}
	else
		{
			printf("-");
		}
	if(mode & S_IXOTH)
		{
			printf("x");
		}
	else
		{
			printf("-");
		}

}

//Operations(commands) //

void add_report(char *district_id, char *role, char *user)
{
	char fisier_sursa[500];
	
	strcpy(fisier_sursa, district_id);
	strcat(fisier_sursa, "/");
	strcat(fisier_sursa, "reports.dat");

	permission_verify(fisier_sursa, role, 'w');
	int fd=open(fisier_sursa, O_WRONLY | O_CREAT | O_APPEND, 0664);
	if(fd==-1)
		{
			perror("Error! reports.dat cannot be opened!");
			exit(-1);
		}
	chmod(fisier_sursa, 0664);
	Report r;
	srand(time(NULL));
	r.id=rand()%100000;
	strcpy(r.inspector_name, user);
	r.timestamp=time(NULL);

	printf("REPORT:\n");
	printf("Enter the GPS coordinates:\n");
	printf("Latitude: ");
	scanf("%f", &r.lat);
	printf("\n");
	printf("Longitude: ");
	scanf("%f", &r.longit);
	printf("\n");
	printf("Enter the Issue Category:\n");
	scanf("%s", r.issue_category);
	printf("\n");
	printf("Enter the Severity Level:\n");
	scanf("%d", &r.severity_level);
	//printf("\n");

	getchar();
	
	printf("Enter your description:\n");
	fgets(r.description, 150, stdin);
	r.description[strcspn(r.description, "\n")]='\0';

	if(write(fd, &r, sizeof(Report))==-1)
	{
		perror("Error! Couldn't write in the report!");
		exit(-1);
	}
	else
	{
		printf("Report saved!\n");
	}
}


void list(char *district_id, char *role)
{
	char fisier_sursa[500];
	strcpy(fisier_sursa, district_id);
	strcat(fisier_sursa, "/reports.dat");
	
	permission_verify(fisier_sursa, role, 'r');

	struct stat st;
	//using the stat() function:
	if(stat(fisier_sursa, &st)==-1)
	  {
		perror("Error! Couldn't get the data!");
		exit(-1);
	   }
	
	printf("FILE DATA:\n");
	printf("Permissions:\n");
	print_permission(st.st_mode);
	printf("\n");
	printf("File size: %lld bytes; Last Modified: %s ", st.st_size, ctime(&st.st_mtime));
	printf("\n");
	
	int fd=open(fisier_sursa, O_RDONLY);
	if(fd==-1)
	{
		perror("Error! couldn't read from file!");
		exit(-1);
	}
	
	Report r;
	int nr=0;

	while(read(fd, &r, sizeof(Report))>0)
	{
		nr++;
		printf("Report no. %d\n", nr);
		printf("ID: ");
		printf("%d\n", r.id);
		printf("Inspector Name: ");
		printf("%s\n", r.inspector_name);
		printf("Coordinates: ");
		printf("%f, %f\n", r.lat, r.longit);
		printf("Issue Category: ");
		printf("%s\n", r.issue_category);
		printf("Severity Level: ");
		printf("%d\n", r.severity_level);
		printf("Timestamp: ");
		printf("%s\n", ctime(&r.timestamp));
		printf("Description: ");
		printf("%s\n", r.description);
	}

	close(fd);
	
}


void view(char *district_id, char *role, int wanted_id)
{
	char fisier_sursa[500];
	strcpy(fisier_sursa, district_id);
	strcat(fisier_sursa, "/reports.dat");
	
	permission_verify(fisier_sursa, role, 'r');
	
	int fd=open(fisier_sursa, O_RDONLY);
	if(fd==-1)
	{
		perror("Error! couldn't read from file!");
		exit(-1);
	}
	
	Report r;
	int ok=0;
	
	while(read(fd, &r, sizeof(Report))>0)
	{
		if(r.id==wanted_id)
		{
			ok=1;
			printf("Inspector Name: ");
			printf("%s\n", r.inspector_name);
			printf("Coordinates: ");
			printf("%f, %f\n", r.lat, r.longit);
			printf("Issue Category: ");
			printf("%s\n", r.issue_category);
			printf("Severity Level: ");
			printf("%d\n", r.severity_level);
			printf("Timestamp: ");
			printf("%s\n", ctime(&r.timestamp));
			printf("Description: ");
			printf("%s\n", r.description);
			printf("---------------------\n");
			break;
		}
	}
	if(ok==0)
	{
		printf("Error! File not found!");
	}
	close(fd);

}


int main(int argc, char *argv[])
{
     	if(argc<5)
		{
	   	perror("Error! invalid number of arguments!");
	   	exit(-1);
		}
	char role[50];
	char user[50];
	char district[50];
	
	if(strcmp(argv[1], "--role")==0)
	{
		strcpy(role, argv[2]);
	}

	if(strcmp(argv[3], "--add")==0)
	{
		strcpy(district, argv[4]);
		create_district_directory(district);
		add_report(district, role, user);
	}
	else if(strcmp(argv[3], "--list")==0)
	{
		strcpy(district, argv[4]);
		list(district, role);
	}
	else if(strcmp(argv[3], "--view")==0)
	{
		strcpy(district, argv[4]);
		int wanted_id=atoi(argv[5]);
		view(district, role, wanted_id);
	}
	else if(strcmp(argv[3], "--user")==0)
	{
		strcpy(user, argv[4]);
		if(strcmp(argv[5], "--add")==0)
		{
			strcpy(district, argv[6]);
			create_district_directory(district);
			add_report(district, role, user);
		}
		else if(strcmp(argv[5], "--list")==0)
		{
			strcpy(district, argv[6]);
			list(district, role);
		}
		else if(strcmp(argv[5], "--view")==0)
		{
			strcpy(district, argv[6]);
			int wanted_id=atoi(argv[7]);
			view(district, role, wanted_id);
		}
		
	}
	else
	{
		printf("Error! invalid command!\n");
	}	
    

    
}

