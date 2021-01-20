#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <iomanip>

using namespace std;

// Function prototype
void readFile();
void vInitialization();
void summaryCalculation();


struct jobInformation // Struct for storing Job Informations
{
	string type; // Store type of job instruction. CPU or I/O.
	int cycle; // Burst time for CPU or I/O.
	int remainingCycle; // The remaining cycle.
	jobInformation *next; // Address for next node.
};

struct ProcessControlBlock // Struct for storing Process Control Block Information
{
	int num;
	int startTime;
	int arrivalTime;
	int pArrival; // Previous Arrival
	int finishTime;
	int turnaroundTime;
	int waitingTime;
	int nofContextSwitching;
	int burstTime;
	int nofInterrupt;
	int	rTime; // Remaining Time
	int status;
};

struct summary  // Struct for storing average turnaround,average waiting time, total interrupt and total context switch
{
	float averageTurnAroundTime;
	float averageWaitingTime;
	int totalNoOfInterrupt;
	int totalNoOfContextSwitching;
}summary;


// Global Variables Declaration
string line;
string fileName = "";

int counter,i,j,check,jobLeft,time;
int timeQuantum = 0;
bool termination;
bool jobStatus;
jobInformation *head[30], *trans, *newnode, *temp;
ProcessControlBlock job[30];
ProcessControlBlock temporary;

class RR{
private:
	struct list{  //Struct for storing list of queue jobs.
		int num;
		list *next;
	};
	int tQuantum;
	list *queue, *process, *temp_q, *first_p;

public:	
	RR(){  //constructor to initialize 
		process = NULL;
		queue = NULL;
		temp = NULL;
		temp_q = NULL;
		first_p = NULL;
		tQuantum=0;
	}
	
	
	void arrivalTimeSort(){   //sort for last job

		for(counter=1;counter<jobLeft;counter++){
			for(i=0;i<30-counter;i++){
				if(job[i].arrivalTime<job[i+1].arrivalTime&&job[i+1].arrivalTime!=-1){
					temporary = job[i];
					job[i]=job[i+1];
					job[i+1]=temporary;
					temp = head[i];
					head[i]= head[i+1];
					head[i+1]= temp ;
					
				}
			}
		}
		for(counter=1;counter<jobLeft;counter++){
			for(i=0;i<30-counter;i++){
				if(job[i].arrivalTime>job[i+1].arrivalTime&&job[i+1].arrivalTime!=-1){
					temporary = job[i];
					job[i]=job[i+1];
					job[i+1]=temporary;
					temp = head[i];
					head[i]= head[i+1];
					head[i+1]= temp ;
				}
				else if(job[i].arrivalTime==job[i+1].arrivalTime&&job[i+1].arrivalTime!=-1){
					if(job[i].pArrival>job[i+1].pArrival){
						temporary = job[i];
						job[i]=job[i+1];
						job[i+1]=temporary;
						temp = head[i];
						head[i]= head[i+1];
						head[i+1]= temp ;
					}
				}
			}
		}
	}

	
	void RRCalculation(int timeQuantum){  //start run RR until all jobs are done
		vInitialization();
		readFile();
		
	cout<<"\t\t\t\t\t\t"<<"Round Robin Scheduling Algorithm"<<endl<<endl;
	cout<<"\t\t\t\t\t\t\t"<<"Time Quantum : "<<timeQuantum<<endl<<endl;
	cout <<"  Job No |   Arrival Time  |   Burst Time  |  Complete Time  |  Context Switching  |  Turnaround Time  |  Waiting Time  |  Interrupt   " <<endl;
		
		termination = false; 
		time =0;
		while(!termination){ //while true, start running job
	start: 
			jobStatus = false; // if false = job not complete and continue the for statement
			for(i=0;i<jobLeft;i++)
			{  
				if(time >= job[i].arrivalTime)//time = system time, if time >= arrival time,  reset job[i].status to 1, prevent duplicate data
				{
					job[i].status = 1;
					jobStatus = true;
				}
			}

			if(!jobStatus) // if jobstatus = false, then go back to start, time++, until jobstatus = true
			{
				time++; 
				goto start;
			}

			if(queue == NULL)// if head = NULL, queue = head, no job in queue, process = job
			{  
				for(i=0;i<jobLeft;i++){
					if(job[i].status ==1){	
						process = new list;
						process->num = job[i].num;
						process->next = NULL;
						if(queue == NULL){ //if queue = NULL, add process into queue
							queue = process;
						}
						else{ // add job into temp_q
							temp_q = queue;
							while(temp_q->next!=NULL){
								temp_q = temp_q->next;
							}
							temp_q->next = process;
						}
					}
				}
			}

			temp_q = queue;
			counter = temp_q->num;
			for(i=0;i<jobLeft;i++){
				if(job[i].num == counter){
					break;
				}
			}

			check = i; // save job data into check

			trans = head[check]; // trans point the data of the head[check] which is jobs data, trans = moving pointer
			jobStatus = false; // false = job not completed, starts the while loop
			tQuantum = 0 ;
			while(tQuantum<timeQuantum){  //time quantum loop
				while(trans->remainingCycle==0){   //traverse instruction that is currently executing, if current instructions rem time = 0, next node != NULL, then trans -> next instruction
					if(trans->next!=NULL){
						trans=trans->next;
					}
					else{				//all instructions are executed and job are completed, point to next, first_p cannot be accessed anymore,save as first_p,delete the first_p then save the first_p at the back
						queue=queue->next; 
						jobStatus = true;
						goto JOB_COMPLETION;
					}
				}

				if(trans->type == "CPU"){ // traverse instruction if time = 1, remainingCycle -1
					time++;
					trans->remainingCycle --;
					for(i=0;i<jobLeft;i++){ // if time increase den need to check if other job had come in during the time
						if(job[i].arrivalTime == time){
							job[i].status = 2; // save to job status 2
						}
					}
					if(trans->remainingCycle==0){ // trans = moving pointer, if trans point to next node = NULL and rem time = 0 , job is completed
						if(trans->next==NULL){
							queue=queue->next;
							jobStatus = true;
							goto JOB_COMPLETION;
						}
					}
				}
				else{
					time++; // system time
					job[check].nofContextSwitching+=1; // switch job, then add 1s of cs
					job[check].pArrival = time;
					job[check].arrivalTime = time+trans->cycle;
					job[check].status=0;
					trans->remainingCycle =0;

					for(i=0;i<jobLeft;i++){
						if(job[i].arrivalTime == time){
							job[i].status = 2;
						}
					}
					break;   //exit CPU early 
				}
				tQuantum++;
			}

			if(trans->type == "CPU"&&head[check]->next!=NULL){  //switch job, add 1s cs if process is CPU
				job[check].nofContextSwitching+=1;
				job[check].nofInterrupt+=1;            
				time++;
			}

			for(i=0;i<jobLeft;i++){                //check if there is other job
				if(job[i].arrivalTime == time){
					job[i].status = 2;
				}
			}
			for(i=0;i<jobLeft;i++){
				if(job[i].status ==2){	
					process = new list;
					process->num = job[i].num;
					process->next = NULL;
					if(queue == NULL){
						queue = process;
					}
					else{
						temp_q = queue;
						while(temp_q->next!=NULL){
							temp_q = temp_q->next;
						}
						temp_q->next = process;
					}
				}
			}

			if(trans->type== "CPU"&&(trans->next!=NULL||trans->remainingCycle!=0)){ // save queue data into first_p, delete first_P, add first_p into the back of the queue
				if(queue->next!=NULL){ 
					first_p = queue;
					process = new list;
					process->num = first_p->num;
					process->next = NULL;
					queue = queue->next;
					delete first_p;
					temp_q = queue;
					while(temp_q->next!=NULL){
						temp_q=temp_q->next;
					}
					temp_q->next = process;	
				}
				else{
					time--;// if next node = NULL, then minus 1 time, cs, no.of.I
					job[check].nofContextSwitching-=1;
					job[check].nofInterrupt-=1;   
				}
			}
			else{  //move to the next job in queue
				queue = queue->next;
			}

			jobStatus = false;
			

	JOB_COMPLETION:if(jobStatus){ // calculate the avg TT, avg WT, avg AT, no.of.CS, no.of.I
					time++;
					job[check].nofContextSwitching+=1;
					job[check].status=0;
					job[check].finishTime = time;
					job[check].arrivalTime = -1;
					job[check].pArrival = -1;
					job[check].turnaroundTime = job[check].finishTime-job[check].startTime;
					job[check].waitingTime=job[check].turnaroundTime-job[check].burstTime-job[check].nofContextSwitching;
	
					cout << setw(6) << job[check].num+1 <<setw(5)  <<" | " << setw(10) << job[check].startTime << setw(8) 
						<<" | " <<setw(9) << job[check].burstTime << setw(7);
					cout <<" | " <<setw(10) << time << setw(8) <<" | " <<setw(11) <<  job[check].nofContextSwitching << setw(11)
						<<" | " <<setw(11) << job[check].turnaroundTime << setw(9);
					cout <<" | " <<setw(10) << job[check].waitingTime << setw(7) <<" | " <<setw(7) << job[check].nofInterrupt <<endl;
				}	

				
				
				if(jobLeft>= 1){ // job go back to sorting function
					arrivalTimeSort();	
				}
				check=0;
				for(i=0;i<30;i++){
					if(job[i].arrivalTime == -1){     
						check++;
					}
				}
				
				jobLeft = 30-check; // check for job left in queue
				if(jobLeft== 0){// if no job left then terminate the algo function 
					termination = true;
				}
		} // end while termination loop
		
			summaryCalculation(); // Calculate the data
		
	}
};

int main(){

	RR R1;
	int cont=1;

	cout <<endl;
	cout<<"Hi, User!"<<endl<<endl;
	cout<<"Welcome to UnimasIdeal Round Robin Calculator Apps"<<endl<<endl;
	cout<<"This program is to read jobs from .csv file and calculate"<<endl;
	cout<<"turnaround time, waiting time, and number of interrupts incured"<<endl;
	cout<<"according to the time quantum entered by the user."<<endl<<endl;

	ifstream file;
	do{
		cout << "Enter the name of .csv file : "<<endl;
		getline(cin, fileName);
		cout << "Enter the time quantum : "<<endl;
		cin>>timeQuantum;
		fileName += ".csv";
		file.open(fileName.c_str());
		if(!file.is_open()){
			cout << "File is not found. "<<endl;
			fflush(stdin);
		}
	}while(!file.is_open());

//Main Menu
	
		MENU:
		system("cls");
		
		R1.RRCalculation(timeQuantum);					// show data of RR
		system("Pause");
		
		while(cont != 0 ){
			cout<<"Do you want to try other quantum time? (1 for Yes, 2 for No) : ";
			cin>>cont;
			system("cls");
			if(cont == 1){
				cout << "Enter the time quantum : "<<endl;
				cin>>timeQuantum;
				R1.RRCalculation(timeQuantum);
			}
			else
			{
				cout<<"Thank you for using our apps! See you again - UnimasIdeal :) ";
				return 0;
			}
			
		}

	return 0;
}

//Function Body

void readFile(){
	ifstream file;
	file.open(fileName.c_str());

	counter = 0;
	for(i=1;i<=120;i++){
		if(i==60 || i==120){
			getline(file, line, '\n');
		}
		else{
			getline(file, line, ',');
		}
		
		if(i%2==0){
			if(i>60){
				stringstream format(line);
				format >> job[counter].arrivalTime;
				job[counter].startTime = job[counter].arrivalTime;
				job[counter].pArrival = job[counter].arrivalTime;
				counter++;
			}	
		}
	}

	i=0;
	j=1;

	while(file.good()){ // loop for jobInformation storage
		getline(file, line, ',');
		j++;

		if(line!=""){
			newnode = new jobInformation;
			newnode->type = line;
			if(j%60!=0){
				getline(file, line, ',');
			}
			else{
				getline(file, line, '\n');
			}

			j++;
			stringstream format(line);
			format >> newnode->cycle;
			newnode->remainingCycle = newnode->cycle;
			newnode->next = NULL;

			if(head[i] == NULL){
				head[i] = newnode;
			}
			else{
				trans = head[i];
				while(trans->next != NULL){		
					trans = trans->next;
				}
				trans->next=newnode;
			}

			i++;
			if(i==30){
				i=0;
			}
		}
	}
	for(i=0;i<30;i++){
		trans = head[i];
		while(trans!=NULL){
			if(trans->type == "CPU"){
				job[i].rTime += trans->cycle;
			}
			job[i].burstTime += trans->cycle;
			trans = trans->next;
		}
	}

	file.close();
}

void vInitialization(){    //intialize variables
	i=0;j=0;time=0;counter=0;
	termination = false;
	check=0;jobLeft=30;
	temporary.arrivalTime=0;
	temporary.finishTime=0;
	temporary.num=0;
	temporary.rTime=0;
	temporary.startTime=0;
	temporary.status=0;
	temporary.turnaroundTime=0;
	temporary.burstTime=0;
	temporary.nofContextSwitching=0;
	temporary.nofInterrupt=0;
	temporary.waitingTime=0;
	temp = NULL;
	
	summary.averageTurnAroundTime=0;
	summary.averageWaitingTime=0;
	summary.totalNoOfContextSwitching=0;
	summary.totalNoOfInterrupt=0;

	for(i=0;i<30;i++){
		head[i] = NULL;
		job[i].num = i;
		job[i].startTime=0;
		job[i].arrivalTime=0;
		job[i].finishTime=0;
		job[i].rTime=0;
		job[i].pArrival=0;
		job[i].status=0;
		job[i].turnaroundTime=0;
		job[i].burstTime=0;
		job[i].nofContextSwitching=0;
		job[i].nofInterrupt=0;
		job[i].waitingTime=0;
	}
}

void summaryCalculation(){   //summary calculation

	for(i=0;i<30;i++){
		summary.averageTurnAroundTime+=job[i].turnaroundTime;
		summary.averageWaitingTime+=job[i].waitingTime;
		summary.totalNoOfContextSwitching+=job[i].nofContextSwitching;
		summary.totalNoOfInterrupt+=job[i].nofInterrupt;
	}

	summary.averageTurnAroundTime = summary.averageTurnAroundTime/30;
	summary.averageWaitingTime = summary.averageWaitingTime/30;

	cout<<"\nAverage turnaround time : "<< summary.averageTurnAroundTime <<endl;
	cout<<"Average waiting time : "<< summary.averageWaitingTime <<endl;
	cout<<"Total context switching : "<< summary.totalNoOfContextSwitching <<endl;
	cout<<"Total interrupt : "<< summary.totalNoOfInterrupt <<endl;
	
}

