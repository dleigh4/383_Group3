
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "node.h"
#include "srt.h"


//Shortest remaining time simulation function
//Prints runtime and performance of algorithm
void srt(PROC *pr, int count) {
	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival)
	//Index correlates to process ID (A -> element 0)
	int start[count], end[count], runtime[count], queue[count];
	int finished = 0;
	int current_quantum = 0;
	int qcount = 0;
	int i = 0;
	int j = 0;
	
	for (int i = 0; i < count; i++) {
		start[i] = 0;
		end[i] = 0;
		runtime[i] = 0;
	}
	
	
	
	
	printf("\nShortest remaining time:\n");
	
	//Main scheduler loop; print process ID for each quantum
	while ((finished < count)) {
		
		//Run process at head of queue, if any
		if (qcount > 0) {
		
			//Mark start of execution if new process
			if (runtime[queue[0]] == 0) {
			
				//If it's past quantum 99 and it's an unrun process, free the process (don't allow new processes into CPU)
				if (current_quantum > 99) {	
					end[queue[0]] = current_quantum + 1;
					qcount--;
					for(j = 0; j < qcount; j++)
						queue[j] = queue[j+1];
					continue;
				} else
					start[queue[0]] = current_quantum;
			
			}
			printf("%c", pr[queue[0]].name);
			runtime[queue[0]]++;
			
			//Check for completion
			if ((float)runtime[queue[0]] >= pr[queue[0]].runtime) {
				end[queue[0]] = current_quantum + 1;
				qcount--;
				for(j = 0; j < qcount; j++)
					queue[j] = queue[j+1];
				
				finished++;
				
			}
			
		}
		else if (current_quantum >= 100)
			break;
		else
			printf("_");
		
		//Add new processes arriving this quantum to the queue
		while ((pr[i].arrivaltime < ((float)current_quantum + 1.0)) && (current_quantum <= 99)) {
		
			//Check whether current entry at iterator arrives this quantum
			if ((pr[i].arrivaltime >= (float)current_quantum)) {
				
				queue[qcount] = i;
				start[i] = current_quantum;
				
				//Case: nonempty queue
				if (qcount >= 1) {
					
					j = qcount - 1;
					
					while ((j > -1) && ((pr[queue[j]].runtime - (float)runtime[queue[j]]) > (pr[i].runtime - (float)runtime[i]))) {
						queue[j + 1] = queue[j];
						queue[j] = i;
						j--;
					}
				
				}
				qcount++;
				i++;
			}
			else
				break;
		}
		
		current_quantum++;
	}
	
	
	//Final process metrics: average turnaround, waiting, and response times
	float turnaround, waiting, response, turnavg, waitavg, responseavg;
	turnavg = 0;
	waitavg = 0;
	responseavg = 0;
	
	//Calculate metrics for each process
	for (int x = 0; x < finished; x++) {
		turnaround = end[x] - pr[x].arrivaltime;
		waiting = turnaround - pr[x].runtime;
		response = start[x] - pr[x].arrivaltime;
		turnavg += turnaround;
		waitavg += waiting;
		responseavg += response;
	}
	
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\nAverage turnaround time: %4.2f \n Average waiting time: %4.2f \n Average response time: %4.2f \n Throughput: %d \n", turnavg, waitavg, responseavg, finished);
	
	
}
	/*
	//Metric arrays: track starting and completion time of each process's execution (not from arrival)
	//Index correlates to process ID (A -> element 0)
	int start[count], end[count], runtime[count];
	int finished = 0;
	int current_quantum = 0;
	int i = 0;
	NODE *head = NULL;
	NODE *tail = NULL;
	NODE *ptr;
	NODE *ptr2;
	
	for (int i = 0; i < count; i++) {
		start[i] = 0;
		end[i] = 0;
		runtime[i] = 0;
	}
	
	
	
	
	printf("\nShortest remaining time:\n");
	
	//Main scheduler loop; print process ID for each quantum
	while ((finished < count)) {
		
		//Run process at head of queue, if any
		if (head != NULL) {
		
			//Mark start of execution if new process
			if (runtime[head->pid] == 0){
			
				//If it's past quantum 99, free the process (don't allow new processes into CPU)
				if (current_quantum > 99) {
					ptr = head;
					head = ptr->tail;
					if (head == NULL)
						tail = NULL;
					free(ptr);
					continue;
				} else
					start[head->pid] = current_quantum;
			}
			
			printf("%c\n", pr[head->pid].name);
			runtime[head->pid]++;
			
			//Check for completion
			if ((float)runtime[head->pid] >= pr[head->pid].runtime) {
			
				printf("freeing\n");
				end[head->pid] = current_quantum + 1;
				ptr = head;
				head = ptr->tail;
				
				if (head == NULL)
					tail = NULL;
				else
					head->head = NULL;
					
				free(ptr);
				finished++;
				
			
			}
			
		}
		else if (current_quantum >= 100)
			break;
		else
			printf("_");
		
		//Add new processes arriving this quantum to the queue
		while ((pr[i].arrivaltime < ((float)current_quantum + 1.0)) && (current_quantum <= 99)) {
		
			//Check whether current entry at iterator arrives this quantum
			if ((pr[i].arrivaltime >= (float)current_quantum)) {
			
				printf("inserting %d\n", i);
				
				ptr = (NODE *)malloc(sizeof(NODE));
				ptr->pid = i;
				ptr->tail = NULL;
				
				//Case: empty queue
				if (head == NULL) {
					printf("empty\n");
					head = ptr;
					tail = ptr;
					ptr->head = NULL;
				}
				//Case: one entry
				else if (head == tail){
					if (pr[head->pid].runtime > pr[ptr->pid].runtime) {
						ptr->tail = head;
						ptr->head = NULL;
						head->head = ptr;
						head = ptr;
					}
					else {
						ptr->head = head;
						ptr->tail = NULL;
						head->tail = ptr;
						tail = ptr;
					}
				}
				//Case: multiple entries
				else {
					//Start at head
					ptr->tail = head;
					ptr->head = NULL;
					head->head = ptr;
					
					if (pr[head->pid].runtime > pr[ptr->pid].runtime)
						head = ptr;
					
					
					//Iterate while unsorted
					while (ptr->tail != NULL) {
						if (pr[ptr->tail->pid].runtime < pr[ptr->pid].runtime) {
							ptr2 = ptr->tail;
							
							ptr2->head = ptr->head;
							ptr->head = ptr2;
							ptr->tail = ptr2->tail;
							ptr2->tail = ptr;
						}
						if (ptr->tail == NULL)
							tail = ptr;
					}
				
				
					printf("entry %d\n", current_quantum);
					//Case: insert at tail (not shorter than tail)
					if ((pr[tail->pid].runtime < pr[ptr->pid].runtime) || head == tail) {
						tail->tail = ptr;
						ptr->head = tail;
						ptr->tail = NULL;
						tail = ptr;
					}
					//Case: shorter than tail
					else {
						ptr->head = tail->head;
						ptr->tail = tail;
						tail->head->tail = ptr;
						tail->head = ptr;
						
						while(ptr->head != NULL) {
							if (ptr->head != NULL)
								if (ptr->head->pid != ptr->pid)
									printf("bubbling head:%c curr:%c %c\n", pr[head->pid].name, pr[ptr->head->pid].name, pr[ptr->pid].name);
							if (pr[ptr->head->pid].runtime < pr[ptr->pid].runtime) 
								break;
								
							ptr2 = ptr->head;
							
							ptr->head = ptr2->head;
							ptr2->tail = ptr->tail;
							ptr2->head = ptr;
							ptr->tail = ptr2;
						}
						
						if (ptr->head == NULL)
							head = ptr;
					}
				}
				i++;
			}
			else
				break;
		}
		
		current_quantum++;
	}
	
	
	//Final process metrics: average turnaround, waiting, and response times
	float turnaround, waiting, response, turnavg, waitavg, responseavg;
	turnavg = 0;
	waitavg = 0;
	responseavg = 0;
	
	//Calculate metrics for each process
	for (int x = 0; x < finished; x++) {
		turnaround = end[x] - pr[x].arrivaltime;
		waiting = turnaround - pr[x].runtime;
		response = start[x] - pr[x].arrivaltime;
		turnavg += turnaround;
		waitavg += waiting;
		responseavg += response;
	}
	
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\nAverage turnaround time: %4.2f \n Average waiting time: %4.2f \n Average response time: %4.2f \n Throughput: %d \n", turnavg, waitavg, responseavg, finished);
	
	
}*/
