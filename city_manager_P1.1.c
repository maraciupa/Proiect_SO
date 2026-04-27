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
   
	//user: //
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
	   if(action=='x' && (st.st_mode & S_IXUSR))
		{
			has_access=1;
		}

	} //group: //
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
	   if(action=='x' && (st.st_mode & S_IXGRP))
		{
			has_access=1;
		}
	}
   else if(action=='r' && (st.st_mode & S_IROTH))
	{
		has_access=1;
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

//for the logged_district file:

void logging_actions(char *district, char *role, char *user, char *action)
{
	char p[500];
	strcpy(p, district);
	strcat(p, "/");
	strcat(p, "logged_district");

	if(strcmp(role, "inspector")==0)
	{
		//perror("Error! the inspector refused your access!");
		return;
	}
	
	int fd=open(p, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if(fd==-1)
	{
		perror("Error! couldn't open for the logged_district file!");
		exit(-1);
	}
	chmod(p, 0664);
	time_t start=time(NULL);
	char *afisare_timp=ctime(&start);
	afisare_timp[strlen(afisare_timp)-1]='\0';
	
	char s[2000];
	int l=sprintf(s, "%s ; %s ; %s; %s\n", afisare_timp, role, user, action);
	write(fd, s, l);
	close(fd);
	
}

//Operations(commands) //

void add(char *district_id)
{
	char p[500]; //path
	
	strcpy(p, district_id);
	strcat(p, "/");
	strcat(p, "reports.dat");

	
	int fd=open(p, O_WRONLY | O_CREAT | O_APPEND, 0664);
	if(fd==-1)
		{
			
			perror("Error! reports.dat cannot be opened!");
			exit(-1);
		}

	chmod(p, 0664);
	Report r;
	srand(time(NULL));
	r.id=rand()%100000;
	r.timestamp=time(NULL);

	printf("REPORT:\n");
	//reading the name of the inspector:
	printf("Inspector name: ");
	scanf("%s", r.inspector_name);
	printf("Enter the GPS coordinates:\n");
	printf("Latitude: ");
	scanf("%f", &r.lat);
	printf("\n");
	printf("Longitude: ");
	scanf("%f", &r.longit);
	printf("\n");
	printf("Enter the Issue Category:\n");
	//scanf("%s", r.issue_category);
	getchar();
	fgets(r.issue_category, 50, stdin);
	r.issue_category[strcspn(r.issue_category, "\n")]='\0';
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


void list(char *district_id)
{
	char p[500]; //path
	strcpy(p, district_id);
	strcat(p, "/reports.dat");
	
	
	struct stat st;
	//using the stat() function:
	if(lstat(p, &st)==-1)
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
	
	int fd=open(p, O_RDONLY);
	if(fd==-1)
	{
		perror("Error! couldn't read from file!");
		exit(-1);
	}
	
	Report r;
	int nr=0;

	

	//reading every report until the end:
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


void view(char *district_id, int report_id)
{
	char p[500]; //path
	strcpy(p, district_id);
	strcat(p, "/reports.dat");
	
	
	int fd=open(p, O_RDONLY);
	if(fd==-1)
	{
		perror("Error! couldn't read from file!");
		exit(-1);
	}
	
	Report r;
	int ok=0;
	
	while(read(fd, &r, sizeof(Report))>0)
	{
		if(r.id==report_id)
		{
			ok=1;
			printf("REPORT # %d:\n", report_id);
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

void remove_report(char *district_id, int wanted_id)
{
	
	char p[500]; //path
	strcpy(p, district_id);
	strcat(p, "/reports.dat");

	struct stat st;
	if(stat(p, &st)==0)
	{
		if((st.st_mode & 0777)!=0664)
		{
			perror("Permission error regarding remove_report!");
			exit(-1);
		}
	}

	int fd=open(p, O_RDWR);
	if(fd==-1)
	{
		perror("Error! couldn't open file!");
		exit(-1);
	}
	chmod(p, 0664);

	Report r;
	int ok=0;
	int poz=0;
	
	//searching for the wanted report and finding the position for it:
	while(read(fd, &r, sizeof(Report))>0)
	{
		if(r.id==wanted_id)
		{
			ok=1;
			poz=lseek(fd, 0, SEEK_CUR)-sizeof(Report);
			break;
		}
	}

	if(ok==0)
	{
		printf("Error! report not found!");
		exit(-1);
	}
	
	//shifting the following reports a position to the left:
	Report next_report;
	long current_poz=poz+sizeof(Report);
	
	while(lseek(fd, current_poz, SEEK_SET)!=-1 && read(fd, &next_report, sizeof(Report))>0)
	{
		lseek(fd, current_poz-sizeof(Report), SEEK_SET);
		write(fd, &next_report, sizeof(Report));
		current_poz=current_poz+sizeof(Report);
	}

	//struct stat st;
	fstat(fd, &st);
	if(ftruncate(fd, st.st_size-sizeof(Report))==-1)
	{
		perror("Error! couldn't truncate the file!");
	}
	else
	{
		printf("Truncate: successfully finished!");
	}

	close(fd);
}


void update_threshold(char *district_id, int value)
{
	int ok=0;

	char p[500]; //path
	strcpy(p, district_id);
	strcat(p, "/district.cfg");


	struct stat st;
	if(stat(p, &st)==0)
	{
		if((st.st_mode & S_IRUSR)==0)
		{
			perror("Error! the owner r bit is not set!");
			exit(-1);
		}

		if((st.st_mode & 0777)!=0640)
		{
			printf("Permission denied! It is not 640!");
			return;
		}
		
		
		//Verify Owner: rw
		if((st.st_mode & S_IRUSR)==0) 
		{
			ok=0;
		}
		if((st.st_mode & S_IWUSR)==0) 
		{
			ok=0;
		}
		if((st.st_mode & S_IXUSR)!=0) 
		{
			ok=0;
		}

		//Verify Group: r
		if((st.st_mode & S_IRGRP)==0) 
		{
			ok=0;
		}
		if((st.st_mode & S_IWGRP)!=0) 
		{
			ok=0;
		}
		if((st.st_mode & S_IXGRP)!=0) 
		{
			ok=0;
		}

		//Verify Others: -
		if((st.st_mode & S_IROTH)!=0) 
		{
			ok=0;
		}
		if((st.st_mode & S_IWOTH)!=0) 
		{
			ok=0;
		}
		if((st.st_mode & S_IXOTH)!=0) 
		{
			ok=0;
		}
		
		if(ok==1)
		{
			printf("Error! Bits do not match 640!");
			
		} 
		
	}

	int fd=open(p, O_WRONLY | O_CREAT | O_TRUNC, 0640);
	if(fd==-1)
	{
		perror("Error! couldn't open the file!");
		exit(-1);
	}
	chmod(p, 0640);
	
	char value_to_string[10];
	sprintf(value_to_string, "%d", value);
	if((write(fd, value_to_string, strlen(value_to_string)))==-1)
	{
		perror("Error! couldn't write in the file!");
		exit(-1);
	}
	else
	{
		printf("Threshold was successfully added to %d!", value);
	}
	close(fd);

}

//The filter command: //

//Parsing function:

int parse_condition(const char *input, char *field, char *op, char *value) {
    // Work on a local copy since strtok modifies the string
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Valid fields for the city management struct
    const char *valid_fields[] = {
        "id", "inspector_name", "lat", "long",
        "category", "severity", "timestamp", "description"
    };
    const int num_fields = 8;

    // Valid operators
    const char *valid_ops[] = { ">=", "<=", "!=", ">", "<", "==" };
    const int num_ops = 6;

    // --- Parsing ---
    char *token;

    // Extract field
    token = strtok(buffer, ":");
    if (token == NULL) return 0;
    strncpy(field, token, 49);
    field[49] = '\0';

    // Extract operator
    token = strtok(NULL, ":");
    if (token == NULL) return 0;
    strncpy(op, token, 3);
    op[3] = '\0';

    // Extract value (allow colons inside value by using the rest of the string)
    token = strtok(NULL, "");
    if (token == NULL) return 0;
    strncpy(value, token, 99);
    value[99] = '\0';

    // --- Validation ---

    // Validate field
    int field_valid = 0;
    for (int i = 0; i < num_fields; i++) {
        if (strcmp(field, valid_fields[i]) == 0) {
            field_valid = 1;
            break;
        }
    }
    if (!field_valid) return 0;

    // Validate operator
    int op_valid = 0;
    for (int i = 0; i < num_ops; i++) {
        if (strcmp(op, valid_ops[i]) == 0) {
            op_valid = 1;
            break;
        }
    }
    if (!op_valid) return 0;

    // Validate value is not empty
    if (value[0] == '\0') return 0;

    return 1;
}

// the match_condition function:

// Helper: applies the operator to two integers
static int compare_int(int a, int b, const char *op) {
    if (strcmp(op, "==")  == 0) return a == b;
    if (strcmp(op, "!=") == 0) return a != b;
    if (strcmp(op, ">")  == 0) return a >  b;
    if (strcmp(op, "<")  == 0) return a <  b;
    if (strcmp(op, ">=") == 0) return a >= b;
    if (strcmp(op, "<=") == 0) return a <= b;
    return 0;
}

// Helper: applies the operator to two floats
static int compare_float(float a, float b, const char *op) {
    if (strcmp(op, "==")  == 0) return a == b;
    if (strcmp(op, "!=") == 0) return a != b;
    if (strcmp(op, ">")  == 0) return a >  b;
    if (strcmp(op, "<")  == 0) return a <  b;
    if (strcmp(op, ">=") == 0) return a >= b;
    if (strcmp(op, "<=") == 0) return a <= b;
    return 0;
}

// Helper: applies the operator to two strings using strcmp
// Only =, != and the inequality operators (lexicographic) are supported
static int compare_string(const char *a, const char *b, const char *op) {
    int cmp = strcmp(a, b);
    if (strcmp(op, "==")  == 0) return cmp == 0;
    if (strcmp(op, "!=") == 0) return cmp != 0;
    if (strcmp(op, ">")  == 0) return cmp >  0;
    if (strcmp(op, "<")  == 0) return cmp <  0;
    if (strcmp(op, ">=") == 0) return cmp >= 0;
    if (strcmp(op, "<=") == 0) return cmp <= 0;
    return 0;
}

int match_condition(Report *r, const char *field, const char *op, const char *value) {
    if (r == NULL || field == NULL || op == NULL || value == NULL)
        return 0;

    // --- int id ---
    if (strcmp(field, "id") == 0) {
        int v = atoi(value);
        return compare_int(r->id, v, op);
    }

    // --- char inspector_name[50] ---
    if (strcmp(field, "inspector_name") == 0) {
        return compare_string(r->inspector_name, value, op);
    }

    // --- float lat ---
    if (strcmp(field, "lat") == 0) {
        float v = strtof(value, NULL);
        return compare_float(r->lat, v, op);
    }

    // --- float longit ---
    if (strcmp(field, "long") == 0) {
        float v = strtof(value, NULL);
        return compare_float(r->longit, v, op);
    }

    // --- char issue_category[50] ---
    if (strcmp(field, "category") == 0) {
        return compare_string(r->issue_category, value, op);
    }

    // --- int severity_level ---
    if (strcmp(field, "severity") == 0) {
        int v = atoi(value);
        return compare_int(r->severity_level, v, op);
    }

    // --- time_t timestamp ---
    // Expected value format: "YYYY-MM-DD HH:MM:SS"

		
    if (strcmp(field, "timestamp") == 0) {
        struct tm tm_val = {0};
        if (sscanf(value, "%d-%d-%d %d:%d:%d",
                   &tm_val.tm_year, &tm_val.tm_mon, &tm_val.tm_mday,
                   &tm_val.tm_hour, &tm_val.tm_min, &tm_val.tm_sec) != 6) {
            // Fallback: try parsing as a raw Unix timestamp integer
            time_t v = (time_t)atoll(value);
            return compare_int((int)r->timestamp, (int)v, op);
        }
        tm_val.tm_year -= 1900;  // tm_year is years since 1900
        tm_val.tm_mon  -= 1;     // tm_mon is 0-indexed
        tm_val.tm_isdst = -1;    // let mktime() figure out DST
        time_t v = mktime(&tm_val);
        if (v == (time_t)-1) return 0;  // conversion failed
        return compare_int((int)r->timestamp, (int)v, op);
    } 
	
    // --- char description[150] ---
    if (strcmp(field, "description") == 0) {
        return compare_string(r->description, value, op);
    }

    // Unknown field
    return 0;
}

//an usual printing/listing function helpful for the filter command (instead of view): //

void print_filtered_report(Report r)
{
	printf("Report ID: %d\n", r.id);
	printf("Inspector Name: %s\n", r.inspector_name);
	printf("Latitude: %f\n", r.lat);
	printf("Longitude: %f\n", r.longit);
	printf("Issue Category: %s\n", r.issue_category);
	printf("Severity Level: %d\n", r.severity_level);
	printf("Timestamp: %s\n", ctime(&r.timestamp));
	printf("Description: %s\n", r.description);
}


//the filter function: //

int filter(Report *r, char *condition)
{
	
	char field[50];
	char operator[15];
	char value[150];
	
	if(!parse_condition(condition, field, operator, value))
	{
		//printf("Error! the parsing didn't succeed!");
		return 0;
	
	}
	
	return match_condition(r, field, operator, value);

}

void verify_all_reports(char *district_id, int argc, char **argv, int x)
{
	char p[500];
	sprintf(p, "%s/reports.dat", district_id);
	int fd=open(p, O_RDONLY);
	if(fd==-1)
	{
		perror("Error! couldn't read from the file!");
		exit(-1);
	}
	Report r;
	while(read(fd, &r, sizeof(Report))>0)
	{
		int match_all=1;
		for(int i=x; i<argc; i++)
		{
			if(!filter(&r, argv[i]))
			{
				match_all=0;
				break;
			}
		}
		if(match_all==1)
		{
			printf("Report who MATCHED the condition(s):\n");
			print_filtered_report(r);
		}
	}
	
	close(fd);
	
	
	
}

//Symbolic links part: //

//a helper function to create the symbolic link for a district: //

void create_symbolic_link(char *district_id)
{
	char p[500];
	char symlink_name[500]; 
	strcpy(p, district_id);
	strcat(p, "/reports.dat");
	
	strcpy(symlink_name, "active_reports-");
	strcat(symlink_name, district_id);

	//if the symbolic link already exists:
	unlink(symlink_name);
	
	if(symlink(p, symlink_name)==0)
	{
		printf("Symlink: %s -> %s\n", symlink_name, p);
	}
	else
	{
		perror("Error! couldn't create the symlink!");

	}
	
}

void verify_existing_symlinks(char *link)
{
	struct stat st;
	if(lstat(link, &st)==0)
	{
		if(S_ISLNK(st.st_mode))
		{
			printf("Symbolic link found! %s\n", link);
		}
		if(stat(link, &st)==-1)
		{
			printf("Warning!! Dangling symlink!");
		}
		else
		{
			printf("Valid symbolic link!");
		}
	}
	
}

int main(int argc, char **argv)
{
     	if(argc<5)
		{
	   	perror("Error! invalid number of arguments!");
	   	exit(-1);
		}
	char role[50]="";
	char user[50]="";
	strcpy(user, "?");
	char district[50]="";

	
	if(strcmp(argv[1], "--role")==0)
	{
		
		strcpy(role, argv[2]);
	}
	
	
	if(strcmp(argv[3], "--user")!=0)
	{
		if(argc<5)
		{
			perror("Error! invalid number of arguments for this case without user!");
			exit(-1);
		}
		strcpy(district, argv[4]);
		

		if(strcmp(argv[3], "--add")==0)
		{
			strcpy(district, argv[4]);
			create_district_directory(district);
			create_symbolic_link(district);
			//creating the path for the permission_verify function:
			char p[500];
			sprintf(p, "%s/reports.dat", district);
			permission_verify(p, role, 'w');
			add(district);
			logging_actions(district, role, user, "ADD_REPORT");
		}
		else if(strcmp(argv[3], "--list")==0)
		{
			
			strcpy(district, argv[4]);
			//creating the path:
			char p[500];
			sprintf(p, "%s/reports.dat", district);
			permission_verify(p, role, 'r');
			list(district);
			logging_actions(district, role, user, "LIST_REPORT");
		}
		else if(strcmp(argv[3], "--view")==0)
		{
			strcpy(district, argv[4]);
			int wanted_id=atoi(argv[5]); //the wanted report id
			//creating the path:
			char p[500];
			sprintf(p, "%s/reports.dat", district);
			permission_verify(p, role, 'r');
			view(district, wanted_id);
			logging_actions(district, role, user, "VIEW");
		}
		else if(strcmp(argv[3], "--update_threshold")==0)
			{
				if(strcmp(role, "manager")!=0)
				{
					printf("Error! Update_threshold is for managers only!");
					exit(-1);
				}
				else
				{
					strcpy(district, argv[4]);
					int val=atoi(argv[5]);
					update_threshold(district, val);
				}
				logging_actions(district, role, user, "UPDATE_THRESHOLD");
			}
		
		
		else if(strcmp(argv[3], "--remove_report")==0)
		{
			if(strcmp(role, "manager")!=0)
			{
				printf("Error! Remove_report is for managers only!");
				exit(-1);
			}
			else
			{
				strcpy(district, argv[4]);
				int wanted_id=atoi(argv[5]);
				//creating the path:
				char p[500];
				sprintf(p, "%s/reports.dat", district);
				permission_verify(p, role, 'w');
				remove_report(district, wanted_id);
			}
			logging_actions(district, role, user, "REMOVE_REPORT");
		}
	
		else if(strcmp(argv[3], "--filter")==0)
		{
			//5 is where the conditions start in the command line:
			verify_all_reports(argv[4], argc, argv, 5);
			logging_actions(district, role, user, "FILTER");
		} 
	}

	else if(strcmp(argv[3], "--user")==0)
		{
			strcpy(user, argv[4]);
			strcpy(district, argv[6]);
			create_symbolic_link(district);
		

			if(strcmp(argv[5], "--add")==0)
			{
				strcpy(district, argv[6]);
				create_district_directory(district);
				//creating the path:
				char p[500];
				sprintf(p, "%s/reports.dat", district);
				permission_verify(p, role, 'w');
				add(district);
				logging_actions(district, role, user, "ADD_REPORT");
			}
			else if(strcmp(argv[5], "--list")==0)
			{
				strcpy(district, argv[6]);
				//creating the path:
				char p[500];
				sprintf(p, "%s/reports.dat", district);
				permission_verify(p, role, 'r');
				list(district);
				logging_actions(district, role, user, "LIST_REPORT");
			}
			else if(strcmp(argv[5], "--view")==0)
			{
				strcpy(district, argv[6]);
				int wanted_id=atoi(argv[7]);
				//creating the path:
				char p[500];
				sprintf(p, "%s/reports.dat", district);
				permission_verify(p, role, 'r');
				view(district, wanted_id);
				logging_actions(district, role, user, "VIEW");
			}
			else if(strcmp(argv[5], "--remove_report")==0)
			{
				if(strcmp(role, "manager")!=0)
				{
					printf("Error! Remove_report is for managers only!");
					exit(-1);
				}
				else
				{
					strcpy(district, argv[6]);
					int wanted_id=atoi(argv[7]);
					//creating the path:
					char p[500];
					sprintf(p, "%s/reports.dat", district);
					permission_verify(p, role, 'w');
					remove_report(district, wanted_id);
				}
				logging_actions(district, role, user, "REMOVE_REPORT");
			}
			else if(strcmp(argv[5], "--update_threshold")==0)
			{
				if(strcmp(role, "manager")!=0)
				{
					printf("Error! Update_threshold is for managers only!");
					exit(-1);
				}
				else
				{
					strcpy(district, argv[6]);
					int val=atoi(argv[7]);
					update_threshold(district, val);
				}
				logging_actions(district, role, user, "UPDATE_THRESHOLD");
			}
		
			else if(strcmp(argv[5], "--filter")==0)
			{
				//7 is where the conditions start in the command line:
				verify_all_reports(argv[6], argc, argv, 7);
				logging_actions(district, role, user, "FILTER");
			} 
		}
		else
		{
			printf("Error! invalid command!\n");
		}	
    

    
}

