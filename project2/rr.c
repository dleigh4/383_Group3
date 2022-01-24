
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "node.h"
#include "stat.h"
#include "rr.h"


//Round robin simulation function
//Prints runtime and performance of algorithm
STAT rr(PROC *pr, int count) {
	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival)
	//Index correlates to process ID (A -> element 0)
	int start[count], end[count], runtime[count];
	int finished = 0;
	int current_quantum = 0;
	int i = 0;
	NODE *head = NULL;
	NODE *tail = NULL;
	NODE *ptr;
	
	for (int i = 0; i < count; i++) {
		start[i] = 0;
		end[i] = 0;
		runtime[i] = 0;
	}
	
	
	
	
	printf("\nRound robin:\n");
	
	//Main scheduler loop; print process ID for each quantum
	while ((finished < count)) {
		
		//Run process at head of queue, if any
		if (head != NULL) {
		
			//Mark start of execution if new process
			if (runtime[head->pid] == 0) {
			
				//If it's past quantum 99 and it's an unrun process, free the process (don't allow new processes into CPU)
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
				
			}
			//If not completed, rotate processes
			else if (head != tail){
				ptr = head;
				head = ptr->tail;
				
				head->head = NULL;
				ptr->head = tail;
				tail->tail = ptr;
				ptr->tail = NULL;
				
				tail = ptr;
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
	
	
	STAT output;
	output.turnaround = turnavg;
	output.waiting = waitavg;
	output.response = responseavg;
	output.throughput = (float)finished;
	
	return output;
}
