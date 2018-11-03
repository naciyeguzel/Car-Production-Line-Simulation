#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lines{
	char line[100];
	char word[30];
};

struct department{
	int depNumber;
	int free;
	int number;
	char name[20];
	int time;
	
	struct car *produced_car;
	struct department *next;
	struct department *previous;
	struct department *down;
}; 

/*This structure is used to create copy of produced car for department list command*/
struct reCar{
	int depNumber;
	char depName[20];
	char name[15];
	char id[4];
	struct reCar *link;
};

struct car{
	int dept;
	char depName[20];
	float result;
	int time;
	int start_time;
	char name[15];
	char id[4];
	struct car *next;
};

struct report{
	int time;
	char type[15];
	char id[4];
};

int count(int);
int find_line_number(char *argv[]);
int find_report_number(char *argv[]);
void read_file(char *argv[], struct lines *);
void addDept(struct lines,struct department **,struct department **);
void printfFactory(struct department *);
void addProduce(struct lines,struct car **);
void addCarList(struct lines,struct car **);
void addProducedCar(struct car *,struct car *,struct car **);
void addReport(struct lines, struct report *, int);
void sort_report(struct report *, int);
void depList(struct department *,struct reCar **);
void min_start_time(struct department *,struct department *, int,struct reCar **);
void lastDepartment(struct department * ,int t, struct car *,struct car **,struct reCar **);
void shiftFinishCar(struct department *, int,struct reCar **);
void firstDepartment(struct department *, int,struct car **);
void produce(struct report *, int,struct car *,struct car **,struct car **,struct reCar **,struct department *,struct department *);
void findReportCommand(struct report*, int , int,struct car **,struct reCar **,struct department *,struct department *);
void printReportCar(struct report, int,struct car **,struct department *);
void updateCarList(int,struct car **,struct department *);
void printReportDepartments(struct department *,struct reCar **);
void printDepartmentLine(int,int);
void printHorizontalLine(struct car *);
void printReportCars(int,struct car **,struct department *);




int main(int argc, char *argv[]){
	
	int i, j=0,line_number=find_line_number(argv);
	struct lines *lines=(struct lines *)malloc(line_number*sizeof( struct lines));
	read_file(argv, lines);
	
	struct department *head=NULL,*last=NULL;
	struct reCar *report_car_head=NULL;
	
	struct car *car_list_head=NULL,*car_head=NULL,*produced_car_head=NULL;
	
	int report_number=find_report_number(argv);
	struct report *reports= (struct report *)malloc(report_number*sizeof(struct report));
	
	/*This loop is used to call functions according to commands*/
	for(i=0;i<line_number;i++){
		
		if(strcmp(lines[i].word,"AddDept")==0)
			addDept(lines[i],&last,&head);
		else if(strcmp(lines[i].word,"PrintFactory")==0)
			printfFactory(head);
		else if(strcmp(lines[i].word,"PrintFactory\n")==0)
			printfFactory(head);
		else if(strcmp(lines[i].word,"Produce")==0){
			addProduce(lines[i],&car_head);
			addCarList(lines[i],&car_list_head);
		}
		else if(strcmp(lines[i].word,"Report")==0){
			addReport(lines[i],reports,j);
			j++;
		}
	}
	
	sort_report(reports,report_number);  
	produce(reports,report_number,produced_car_head,&car_head,&car_list_head,&report_car_head,last,head);
	return 0;
	
}

/*This function is used to count number of digits in an integer and it returns count number of digits*/
int count(int number){
	int count=0;
	while(number!=0){
		number/=10;
		count++;
	}
	return count;
}

/*This function is used to find total line number from the read file and it returns total line number*/
int find_line_number(char *argv[]){
	
	FILE *fptr;
	int lineNumber=0;
	
    if((fptr=fopen(argv[1],"r"))==NULL){
        printf("there is not a file..");
        exit(1);
    }
    else{
    	char line[100];
        /*This loop count number of all lines*/
        while(fgets(line, sizeof(line), fptr)) {
        	lineNumber++;
		} 
		
        fclose(fptr);
    }
    return lineNumber;
}

/*This function is used to find total report number from the read file and it returns total report number*/
int find_report_number(char *argv[]){
	
	FILE *fptr;
	int reportNumber=0;
	
    if((fptr=fopen(argv[1],"r"))==NULL){
        printf("there is not a file..");
        exit(1);
    }
    else{
    	char line[100];
    	char *word=(char*)malloc(30);
        /*This loop count number of all Report*/
        while(fgets(line, sizeof(line), fptr)) {
        	word= strtok (line," ");
        	if(strcmp("Report",word)==0){
        		reportNumber++;
			}
		} 
		
        fclose(fptr);
    }
    return reportNumber;
}

/*This function is used to read file and gets all line to lines array*/
void read_file(char *argv[], struct lines *lines){
	FILE *fptr;
    char line[100];
    
	if((fptr=fopen(argv[1],"r"))==NULL){
        printf("there is not a file..");
        exit(1);
    }
    
    else{
    	char *word=(char*)malloc(90);
    	int i=0;
        /*read file line by line and get lines*/
        while(fgets(line, sizeof(line), fptr)) {
        	if(strcmp(line,"\r\n")!=0||strcmp(line,"\n")!=0){
        		strcpy(lines[i].line,line);
                word= strtok (line," \r");
                strcpy(lines[i].word,word);
                i++;
			}
		} 
        fclose(fptr);
    }	
}

/*This function is used to create department list which is triple according to AddDept command argumants.*/
void addDept(struct lines line,struct department **last,struct department **head){
	/*AddDept command argumants stored in department struct variables*/
	struct department *newDepartment= (struct department*)malloc(sizeof(struct department));
    sscanf(line.line, "%s %d %s %d",line.word,&(newDepartment->number),newDepartment->name,&(newDepartment->time));
    newDepartment->free=0;
    newDepartment->depNumber=1;
    
    newDepartment->produced_car=NULL;
    newDepartment->next=NULL;
    newDepartment->down=NULL;
    struct department *temp=newDepartment;
    int i=1,r=2;
    
    /*this block is used to do sibling deparments.*/
    while(i<newDepartment->number){
    	/*AddDept command argumants stored in department struct variables*/
    	struct department *subDepartment= (struct department*)malloc(sizeof(struct department));
    	sscanf(line.line, "%s %d %s %d",line.word,&(subDepartment->number),subDepartment->name,&(subDepartment->time));
    	
    	subDepartment->free=0;
    	subDepartment->depNumber=r;
    	
    	subDepartment->produced_car=NULL;
    	subDepartment->down=NULL;
    	
		temp->down=subDepartment;
		temp=temp->down;
		i++;
		r++;
	}
	
    /*This block is used to do double linked list of depatments.*/
    if(*head==NULL){
    	newDepartment->previous=NULL;
    	*head=newDepartment;
    	*last=newDepartment;
	}
	else{
		(*last)->next=newDepartment;
		newDepartment->previous =*last;
		*last=newDepartment;
	}  
	printf("Department %s has been created.\n",newDepartment->name);
}

/*This function is used to print department double list and their siblings from factory in to screen*/
void printfFactory(struct department *head){
	
	struct department *temp=head;
    int j,space=0;
    /*This block is used to print departments*/
	while(temp!=NULL){
		j=1;
		
		printf("- %s%d",temp->name,j);
		space=space+2+strlen(temp->name)+count(j);
		int i=1;
		struct department *a=temp;
		j++;
		
		/*This block is used to print siblings of departments*/
		while(i<temp->number){
			
		    printf(" %s%d",a->name,j);
		    space=space+1+strlen(a->name)+count(j);
		    a=a->down;
		    i++;
		    j++;
		}
		temp=temp->next;
		printf("\n");
		
		if(temp!=NULL){
			int k=0;
			while(k<space){
				printf(" ");
			    k++;
		    }
    	}
	}
	printf("\n");
}

/*This function is used to create car list which will be produced according to Produce command argumants.*/
void addProduce(struct lines line,struct car **car_head){
	
	struct car *car= (struct car*)malloc(sizeof(struct car));
    sscanf(line.line, "%s %d %s %s",line.word,&(car->time),car->name,car->id);
    car->next=NULL;
	
	/*This block is used to create car linked list.*/
    if(*car_head==NULL){
    	*car_head=car;
	}
	else{
		struct car *temp=*car_head;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=car;
	}  
}

/*This function is used to create second car list which will be produced according to Produce command argumants.
deletion is not performed after the list is created so that status of all cars is protected in the list*/
void addCarList(struct lines line,struct car **car_list_head){
	
	struct car *car= (struct car*)malloc(sizeof(struct car));
    sscanf(line.line, "%s %d %s %s",line.word,&(car->time),car->name,car->id);
    /*This 0 value means that the car is produce list, it is not started to produce*/
    car->dept=0;
    car->next=NULL;
	
	/*This block is used to create car linked list.*/
    if(*car_list_head==NULL){
    	*car_list_head=car;
	}
	else{
		struct car *temp=*car_list_head;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=car;
	}  
}

/*This function is used to create car list which will be finished to produce over time*/
void addProducedCar(struct car *car, struct car *produced_car_head,struct car **car_list_head){
	/*This 2 value means that the car is not department linked list, it is finished to produce*/
	car->dept=2;
	
	/*update car list which has got all cars.*/
	struct car *temp=*car_list_head;
	while(temp!=NULL){
		if(strcmp(temp->id,car->id)==0){
			temp->dept=car->dept;
			temp->result=100.00;
		}
		temp=temp->next;
	}
	
	car->next=NULL;
	
	/*This block is used to do produced car linked list.*/
    if(produced_car_head==NULL){
    	produced_car_head=car;
	}
	else{
		struct car *temp=produced_car_head;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=car;
	}
}

/*This function is used to create report array which will be reported according to report command argumants.*/
void addReport(struct lines line, struct report *reports, int j){
	sscanf(line.line, "%s %d %s %s",line.word,&(reports[j].time),reports[j].type,reports[j].id);
}

/*This function is used to sort the reports according to T time from report array in an ascending order*/
void sort_report(struct report *reports, int report_number){
	int i,j;
	struct report temp;
	for (i = 0; i < report_number; ++i){
		for (j = i + 1; j < report_number; ++j){
			if (reports[i].time > reports[j].time){
				temp=reports[i];
				reports[i] =reports[j];
				reports[j] =temp;
			}
		}
	} 
}

/*This function is used to create produced car list for each depatment. It adds to list department and car information.
The list is used report for Departments command for each department, cars are printed according to department.*/
void depList(struct department *minumum,struct reCar **report_car_head){
	if(*report_car_head==NULL){
		*report_car_head=(struct reCar *)malloc(sizeof(struct reCar));
		strcpy((*report_car_head)->depName,minumum->name);
		(*report_car_head)->depNumber=minumum->depNumber;
		strcpy((*report_car_head)->id,minumum->produced_car->id);
		strcpy((*report_car_head)->name,minumum->produced_car->name);
		(*report_car_head)->link=NULL;
	}
	else{
		struct reCar *tmp=*report_car_head;
				
		while(tmp->link!=NULL){
			tmp=tmp->link;
		}
		tmp->link=(struct reCar *)malloc(sizeof(struct reCar));
		tmp->link->link=NULL;
		strcpy(tmp->link->depName,minumum->name);
		tmp->link->depNumber=minumum->depNumber;
		strcpy(tmp->link->id,minumum->produced_car->id);
		strcpy(tmp->link->name,minumum->produced_car->name);
					
	}
}

/*This function is used to find real minimumum start time of cars from previous department,
call depList function to add produced cars of departments, free department when car production end
and add car to next free department*/
void min_start_time(struct department *pre_vertical,struct department *vertical,int t,struct reCar **report_car_head){
	
	struct department *copy_pre_vertical=pre_vertical;
	struct department *minumum;
	
	while(pre_vertical!=NULL){
		if(pre_vertical->free==1){
			int passingTime=(t-(pre_vertical->produced_car->start_time));
			
			/*checks whether the production time is over*/
			if(passingTime>=pre_vertical->time){
				minumum=pre_vertical;
				
				/*This block is used to find real minimum start time of cars from previous departmen*/
				while(copy_pre_vertical!=NULL){
					
					if(copy_pre_vertical->free==1){
						passingTime=(t-(copy_pre_vertical->produced_car->start_time));
						
						/*checks whether the production time is over*/
						if(passingTime>=copy_pre_vertical->time){
							if(copy_pre_vertical->produced_car->time<minumum->produced_car->time){
								minumum=copy_pre_vertical;
							}
						}
					}
					
				    copy_pre_vertical=copy_pre_vertical->down;
			    }
			    /*This function is called to add produced cars of departments*/
			    depList(minumum,report_car_head);
				
			    vertical->produced_car=minumum->produced_car;
			    vertical->produced_car->start_time=t;
			    vertical->free=1;
			    
			    minumum->produced_car=NULL;
			    minumum->free=0;
			    break;
			}
		}
		pre_vertical=pre_vertical->down;
	}
}

/*This block is used to call addProducedCar function which creates produced car list
and free last department when car production end*/
void lastDepartment(struct department *vertical,int t,struct car *produced_car_head,struct car **car_list_head,struct reCar **report_car_head){
	while(vertical!=NULL){
		if(vertical->free==1){
			int passingTime=(t-(vertical->produced_car->start_time));
			/*checks whether the production time is over*/
			if(passingTime>=vertical->time){
				
				depList(vertical,report_car_head);
				
				addProducedCar(vertical->produced_car,produced_car_head,car_list_head);
		    	vertical->free=0;
		    	vertical->produced_car=NULL;
			}
		}
		vertical=vertical->down;
    }
}

/*This function is used to free departments and transfer cars from the last towards the head*/ 
void shiftFinishCar(struct department *horizontal, int t,struct reCar **report_car_head){
	while(horizontal->previous!=NULL){
		struct department *vertical=horizontal;
			
		while(vertical!=NULL){
			/*checks whether the department is free to transfer car from previous department*/
			if(vertical->free==0){
				struct department *pre_vertical=horizontal->previous;
				/*This function is called to find real minimumum start time of cars*/
				min_start_time(pre_vertical,vertical,t,report_car_head);
			}
			vertical=vertical->down;
		}
		horizontal=horizontal->previous;
	}
}

/*This function is used to get car from produce list to head department which are free to get cars.*/
void firstDepartment(struct department *vertical, int t,struct car **car_head){
	
	struct car *temp;
	
	while(vertical!=NULL){
		if(vertical->free==0){
			/*Check whether car list free*/
			if(*car_head!=NULL){
				temp=*car_head;
				/*Check whether the production time of the car is appropriate*/
				if(t>=temp->time){
					vertical->produced_car=temp;
					vertical->produced_car->start_time=t;
					
					*car_head=(*car_head)->next;
                    temp->next=NULL;
				    vertical->free=1;
				}
			}
		}
		vertical=vertical->down;
    }
}

/*This function is used to call functions which are lastDepartment, shiftFinishCar, firstDepartment 
and findReportCommand according to increasing t.*/
void produce(struct report* reports, int report_number,struct car *produced_car_head, struct car **car_head,
struct car **car_list_head,struct reCar **report_car_head,struct department *last,struct department *head){
	int t;
    
    /*for is increased according to the biggest t from report command*/
	for(t=0;t<=(reports[report_number-1].time+1);t++){
		
		/*This function is called to create produced car list and to free the last department*/
		lastDepartment(last,t,produced_car_head,car_list_head,report_car_head);
		
		/*This function is called to free departments and transfer cars from the last towards the head*/
		shiftFinishCar(last,t,report_car_head);
		
		/*This function is called to get car from produce list to head department.*/
		firstDepartment(head,t,car_head);
    	
    	/*This function is called to decide polymorphic report command*/
    	findReportCommand(reports, report_number, t,car_list_head,report_car_head,last,head);
	}
}

/*This function is used to find report command in process time from report array, then it decide polymorphic 
report command and call function to print according to ascending time report command*/
void findReportCommand(struct report* reports, int report_number, int t,struct car **car_list_head,
struct reCar **report_car_head,struct department *last,struct department *head){
	
	int b;
	for(b=0;b<report_number;b++){
		if(reports[b].time==(t-1)){
			updateCarList(t,car_list_head,head);
			
			if(strcmp(reports[b].type,"Car")==0){
				printf("Command: Report Car %d %s\n",reports[b].time,reports[b].id);
    			printReportCar(reports[b],t,car_list_head,head);
    			printf("\n");
			}
			if(strcmp(reports[b].type,"Cars")==0){
				printf("Command: Report Cars %d\n",reports[b].time);
				printReportCars(t,car_list_head,head);
				printf("\n");
			}
			if(strcmp(reports[b].type,"Departments")==0){
				printf("Command: Report Departments %d\n",reports[b].time);
				printReportDepartments(last,report_car_head);
			}
		}
	}
}

/*This function is used to find a special car from car list according to report car command,
then it prints status of car*/
void printReportCar(struct report report, int t,struct car **car_list_head,struct department *head){
	
	struct car *temp=*car_list_head;
	while(temp!=NULL){
		/*check whether the car is find in car list.*/
		if(strcmp(temp->id,report.id)==0){
		
		printHorizontalLine(temp);
		printf("|Report for %s %s|\n",temp->name,temp->id);
		printHorizontalLine(temp);
	
		/*This block is used to print status of the car which is not started to produce*/
		if(temp->dept==0){
			struct department *horiz=head;
			while(horiz!=NULL){
				printf("%s:0, ",horiz->name);
				horiz=horiz->next;
			}
			printf("| Start Time: %d | Complete:%.2f%% | ",temp->time,temp->result);
			printf("Not complete");
			printf("\n");
		}
		/*This block is used to print status of the car which is not finished to produce*/
		else if(temp->dept==1){
			struct department *horiz=head;
			int totalTime=0,passTotalTime=0;
			
			/*This block is used to print status of the car in passing departmants*/
			while(strcmp(horiz->name,temp->depName)!=0){
				printf("%s:%d, ",horiz->name,horiz->time);
				
				totalTime=totalTime+horiz->time;
				passTotalTime=passTotalTime+horiz->time;
				horiz=horiz->next;
			}
			
			/*This block is used to calculte passing time of the car in current deparment*/
			int pas_time=t-temp->start_time;
			if(pas_time>horiz->time){
				pas_time=horiz->time;
			}
			
			printf("%s:%d, ",horiz->name,pas_time);
			totalTime=totalTime+horiz->time;
			passTotalTime=passTotalTime+pas_time;
			
			horiz=horiz->next;
			/*This block is used to print status of the car in not passing departmants*/
			while(horiz!=NULL){
				printf("%s:0, ",horiz->name);
				totalTime=totalTime+horiz->time;
				horiz=horiz->next;
			}
			temp->result=100*(float)passTotalTime/totalTime;
			
			printf("| Start Time: %d | Complete:%.2f%% | ",temp->time,temp->result);
			printf("Not complete");	
			printf("\n");
		}
		/*This block is used to print status of the car which is finished to produce*/
		else if(temp->dept==2){
			struct department *horiz=head;
			while(horiz!=NULL){
				printf("%s:%d, ",horiz->name,horiz->time);
				horiz=horiz->next;
			}
			printf("| Start Time: %d | Complete:100%% | ",temp->time,temp->result);
			printf("Complete");
			printf("\n");
		}
		
	    }
	temp=temp->next;
	}
}

/*This function is used to find cars which do not complete status of all department from department
and update car list according to by changing values*/
void updateCarList(int t,struct car **car_list_head,struct department *head){
	struct department *horiz=head;
	
	while(horiz!=NULL){
		struct department *vert=horiz;
		
		while(vert!=NULL){
			if(vert->free==1){
				/*This 1 value means that the car is department linked list, it is not finished to produce*/
				vert->produced_car->dept=1;
				struct car *temp=*car_list_head;
				
				/*update car list which has got all cars.*/
				while(temp!=NULL){
					if(strcmp(temp->id,vert->produced_car->id)==0){
						temp->dept=1;
						strcpy(temp->depName,vert->name);
						temp->start_time=vert->produced_car->start_time;
					}
					temp=temp->next;
				}
			}
			vert=vert->down;
		}
		horiz=horiz->next;
	}
}

/*This function is used to print produced cars for each departments and status of all department in factory*/
void printReportDepartments(struct department *horizontal,struct reCar **report_car_head){
	
	while(horizontal!=NULL){
		struct department *vertical=horizontal;
		
		while(vertical!=NULL){
			
			printDepartmentLine(strlen(vertical->name),count(vertical->depNumber));
		    printf("|Report for Department \"%s %d\"|\n",vertical->name,vertical->depNumber);
		    printDepartmentLine(strlen(vertical->name),count(vertical->depNumber));
		    
		    if(vertical->produced_car==NULL){
		    	printf("%s %d is now free.\n",vertical->name,vertical->depNumber);
			}
			if(vertical->produced_car!=NULL){
				printf("I am currently processing %s %s\n",vertical->produced_car->name, vertical->produced_car->id);
			}
			
			struct reCar *tmp=*report_car_head;
			while(tmp!=NULL){
				if(strcmp(tmp->depName,vertical->name)==0&&tmp->depNumber==vertical->depNumber){
					printf("Processed Cars\n");
					break;
				}
				tmp=tmp->link;
			}
			/*This block is used if there is, to print Processed Cars for each departments*/
			tmp=*report_car_head;
			int z=1;
			while(tmp!=NULL){
				if(strcmp(tmp->depName,vertical->name)==0&&tmp->depNumber==vertical->depNumber){
					printf("%d. %s %s\n",z,tmp->name,tmp->id);
					z++;
				}
				tmp=tmp->link;
			}
			
			vertical=vertical->down;
		}
		horizontal=horizontal->previous;
	}
}

/*This function is used to print horizontal line for department*/
void printDepartmentLine(int len, int len_digit){
	int y,count=27+len_digit+len;
	for(y=0;y<count;y++){
		printf("-");
	}
	printf("\n");
}

/*This function is used to print horizontal line for report car*/
void printHorizontalLine(struct car *temp){
	int y,count=18+strlen(temp->name);
	for(y=0;y<count;y++){
		printf("-");
	}
	printf("\n");
}

/*This function is used to print status of all cars from car list for report cars command*/
void printReportCars(int t,struct car **car_list_head,struct department *head){
	struct car *temp=*car_list_head;
	while(temp!=NULL){
		
		printHorizontalLine(temp);
		printf("|Report for %s %s|\n",temp->name,temp->id);
		printHorizontalLine(temp);
	
	    /*This block is used to print status of the cars which is not started to produce*/
		if(temp->dept==0){
			struct department *horiz=head;
			while(horiz!=NULL){
				printf("%s:0, ",horiz->name);
				horiz=horiz->next;
			}
			printf("| Start Time: %d | Complete:%.2f%% | ",temp->time,temp->result);
			printf("Not complete");
			printf("\n");
		}
		/*This block is used to print status of the cars which is not finished to produce*/
		else if(temp->dept==1){
			struct department *horiz=head;
			int totalTime=0,passTotalTime=0;
			
			/*This block is used to print status of the cars in passing departmants*/
			while(strcmp(horiz->name,temp->depName)!=0){
				printf("%s:%d, ",horiz->name,horiz->time);
				
				totalTime=totalTime+horiz->time;
				passTotalTime=passTotalTime+horiz->time;
				horiz=horiz->next;
			}
			
			/*This block is used to calculte passing time of the cars in current deparment*/
			int pas_time=t-temp->start_time;
			if(pas_time>horiz->time){
				pas_time=horiz->time;
			}
			
			printf("%s:%d, ",horiz->name,pas_time);
			totalTime=totalTime+horiz->time;
			passTotalTime=passTotalTime+pas_time;
			
			horiz=horiz->next;
			
			/*This block is used to print status of the cars in not passing departmants*/
			while(horiz!=NULL){
				printf("%s:0, ",horiz->name);
				totalTime=totalTime+horiz->time;
				horiz=horiz->next;
			}
			temp->result=100*(float)passTotalTime/totalTime;
			
			printf("| Start Time: %d | Complete:%.2f%% | ",temp->time,temp->result);
			printf("Not complete");	
			printf("\n");
		}
		/*This block is used to print status of the cars which is finished to produce*/
		else if(temp->dept==2){
			struct department *horiz=head;
			while(horiz!=NULL){
				printf("%s:%d, ",horiz->name,horiz->time);
				horiz=horiz->next;
			}
			printf("| Start Time: %d | Complete:100%% | ",temp->time,temp->result);
			printf("Complete");
			printf("\n");
		}
		temp=temp->next;
	}
}

