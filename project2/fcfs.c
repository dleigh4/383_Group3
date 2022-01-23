
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "node.h"
#include "fcfs.h"


//First-come first-served simulation function
//Prints runtime and performance of algorithm
void fcfs(PROC *pr, int count) {
	
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
	
	for (int j = 0; j < count; j++) {
		start[j] = 0;
		end[j] = 0;
		runtime[j] = 0;
	}
	
	
	
	
	printf("\nFirst-come first-served:\n");
	
	//Main scheduler loop; print process ID for each quantum
	while ((finished < count)) {
		
		//Run process at head of queue, if any
		if (head != NULL) {
		
			//Mark start of execution if new process
			if (runtime[head->pid] == 0)
				start[head->pid] = current_quantum;
			
			printf("%c", pr[head->pid].name);
			runtime[head->pid]++;
			
			//Check for completion
			if ((float)runtime[head->pid] >= pr[head->pid].runtime) {
				end[head->pid] = current_quantum + 1;
				ptr = head;
				head = ptr->tail;
				if (head == NULL)
					tail = NULL;
				free(ptr);
				finished++;
				
				//If past quantum 99, free rest of queue
				if (current_quantum > 99) {
					ptr = head;
					while (ptr != NULL) {
						ptr2 = ptr->tail;
						free(ptr);
						ptr = ptr2;
					}
					break;
				}
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
				
				ptr = (NODE *)malloc(sizeof(NODE));
				ptr->pid = i;
				ptr->tail = NULL;
				
				//Case: empty queue
				if (head == NULL) {
					head = ptr;
					tail = ptr;
					ptr->head = NULL;
				}
				//Case: tail
				else {
					tail->tail = ptr;
					ptr->head = tail;
					tail = ptr;
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
	
	
}
