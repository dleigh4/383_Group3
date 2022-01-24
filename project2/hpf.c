
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "node.h"
#include "stat.h"
#include "hpf.h"



//Highest priority first simulation function (both versions)
//Prints runtime and performance of algorithm
STAT *hpf(PROC *pr, int count) {
	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival) as well as total processes completed
	//Index correlates to process ID (A -> element 0)
	int start[count], end[count], completed, quantum;
	int age[count];
	int runtime[count];
	NODE *ptr, *ptr2;
	
	for (int i = 0; i < count; i++) {
		start[i] = -1;
		end[i] = 0;
		age[i] = 0;
		runtime[i] = 0;
	}
	
	//NON-PREEMPTIVE
	printf("\nHighest priority first (non-preemptive):\n");
	
	//Schedule queues: track performance across each as well as number of processes queued
	NODE *qhead[PRIORITY_LIMIT];
	NODE *qtail[PRIORITY_LIMIT];
	int qcount[PRIORITY_LIMIT];
	
	for (int j = 0; j < PRIORITY_LIMIT; j++) {
		qhead[j] = NULL;
		qtail[j] = NULL;
		qcount[j] = 0;
	}
	
	//Aging and runtime: track time in current queue and overall execution time per-process
	int current_proc = -1;
	int i, j, k, current_prio;
	int it = 0;
	
	//Main scheduler loop; print process ID for each quantum
	completed = 0;
	quantum = 0;
	
	while (completed < count) {
		
		//Run process
		if (current_proc != -1) {
		
			//Mark start of execution if new process
			if (runtime[current_proc] == 0)
				start[current_proc] = quantum;
				
			printf("%c", pr[current_proc].name);
			runtime[current_proc]++;
		
			//Remove from queue if complete
			if ((float)runtime[current_proc] >= pr[current_proc].runtime) {
				completed++;
				qcount[current_prio]++;
				end[current_proc] = quantum + 1;
				
				ptr = qhead[current_prio];
				qhead[current_prio] = ptr->tail;
				if (qhead[current_prio] == NULL)
					qtail[current_prio] = NULL;
				free(ptr);
				
				current_proc = -1;
				
				//If past quantum 99, free rest of queued processes
				if (quantum > 99) {
					for (i = 0; i < PRIORITY_LIMIT; i++) {
						ptr = qhead[i];
						while (ptr != NULL) {
							ptr2 = ptr->tail;
							free(ptr);
							ptr = ptr2;
						}
						qhead[i] = NULL;
						qtail[i] = NULL;
					}
					break;
				}
			}
		}
		else
			printf("_");
		
		//Check whether any processes arrived this quantum; if so, initialize them and add them to their appropriate queue
		while ((pr[it].arrivaltime < ((float)quantum + 1.0)) && (quantum <= 99)) {
			//Check whether current entry at iterator arrives this quantum
			if ((pr[it].arrivaltime >= (float)quantum)) {
				
				ptr = (NODE *)malloc(sizeof(NODE));
				ptr->pid = it;
				ptr->tail = NULL;
				
				//Case: empty queue
				if (qhead[pr[it].priority - 1] == NULL) {
					qhead[pr[it].priority - 1] = ptr;
					qtail[pr[it].priority - 1] = ptr;
					ptr->head = NULL;
				}
				//Case: tail
				else {
					qtail[pr[it].priority - 1]->tail = ptr;
					ptr->head = qtail[pr[it].priority - 1];
					qtail[pr[it].priority - 1] = ptr;
				}
				
				it++;
			}
			else
				break;
		}
		
		quantum++;
		
		
		//If there's no current process, select one to run next quantum 
		if (current_proc == -1) {
			//Iterate through queues to find a process
			for(j = 0; j < PRIORITY_LIMIT; j++) {
				if (qhead[j] != NULL) {
					current_proc = qhead[j]->pid;
					current_prio = j;
					break;
				}
			}
		}
	}
	
	//Final process metrics: average turnaround, waiting, and response times
	float turnaround, waiting, response;
	float turnavg, waitavg, responseavg;
	
	float qturnavg[PRIORITY_LIMIT], qwaitavg[PRIORITY_LIMIT], qresponseavg[PRIORITY_LIMIT];
	
	//Initialize per-queue metrics
	for (int v = 0; v < PRIORITY_LIMIT; v++) {
		qturnavg[v] = 0.0;
		qwaitavg[v] = 0.0;
		qresponseavg[v] = 0.0;
	}
	
	//Calculate metrics for each process
	for (int x = 0; x < count; x++) {
		if (runtime[x] > 0) {
			turnaround = (float)end[x] - pr[x].arrivaltime;
			waiting = turnaround - pr[x].runtime;
			response = (float)start[x] - (float)pr[x].arrivaltime;
			turnavg += turnaround;
			waitavg += waiting;
			responseavg += response;
			
			qturnavg[pr[x].priority - 1] += turnaround;
			qwaitavg[pr[x].priority - 1] += waiting;
			qresponseavg[pr[x].priority - 1] += response;
		}
	}
	
	
	STAT *output = (STAT *)malloc(sizeof(STAT) * (PRIORITY_LIMIT + 1) * 2);
	
	for (int y = 0; y < PRIORITY_LIMIT; y++) {
		qturnavg[y] /= qcount[y];
		qwaitavg[y] /= qcount[y];
		qresponseavg[y] /= qcount[y];
		printf("\n Priority %d average turnaround time: %4.2f \n Priority %d average waiting time: %4.2f \n Priority %d average response time: %4.2f \n Priority %d throughput: %d \n", (y + 1), qturnavg[y], (y + 1), qwaitavg[y], (y + 1), qresponseavg[y], (y + 1), qcount[y]);
		
		output[y].turnaround = qturnavg[y];
		output[y].waiting = qwaitavg[y];
		output[y].response = qresponseavg[y];
		output[y].throughput = qcount[y];
	}
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\n Overall average turnaround time: %4.2f \n Overall average waiting time: %4.2f \n Overall average response time: %4.2f \n Overall throughput: %d \n", turnavg, waitavg, responseavg, completed);
	
	output[PRIORITY_LIMIT].turnaround = turnavg;
	output[PRIORITY_LIMIT].waiting = waitavg;
	output[PRIORITY_LIMIT].response = responseavg;
	output[PRIORITY_LIMIT].throughput = completed;
	
	
	
	
	
	//PREEMPTIVE
	
	for (i = 0; i < count; i++) {
		start[i] = -1;
		end[i] = 0;
		age[i] = 0;
		runtime[i] = 0;
	}
	
	printf("\nHighest priority first (preemptive):\n");
	
	//Schedule queues: track performance across each as well as number of processes queued
	for (int j = 0; j < PRIORITY_LIMIT; j++) {
		qhead[j] = NULL;
		qtail[j] = NULL;
		qcount[j] = 0;
	}
	
	//Aging and runtime: track time in current queue and overall execution time per-process
	current_proc = -1;
	it = 0;
	
	//Main scheduler loop; print process ID for each quantum
	completed = 0;
	quantum = 0;
	
	while (completed < count) {
		
		//Run process
		if (current_proc != -1) {
		
			//Mark start of execution if new process
			if (runtime[current_proc] == 0) {
			
				//If it's past quantum 99 and it's an unrun process, free the process (don't allow new processes into CPU)
				if (quantum > 99) {
					ptr = qhead[current_prio];
					qhead[current_prio] = ptr->tail;
					if (qhead[current_prio] == NULL)
						qtail[current_prio] = NULL;
					free(ptr);
					
					//Choose which queue to probe next iteration
					for (k = 0; k < PRIORITY_LIMIT; k++) {
						if (qhead[k] != NULL) {
							current_proc = qhead[k]->pid;
							current_prio = k;
						}
					}
					//Stop execution if all queues are empty
					if (k == PRIORITY_LIMIT)
						break;
					continue;
				} else
					start[current_proc] = quantum;
				
				
			}
				
			printf("%c", pr[current_proc].name);
			runtime[current_proc]++;
		
			//Remove from queue if complete
			if ((float)runtime[current_proc] >= pr[current_proc].runtime) {
				completed++;
				qcount[current_prio]++;
				end[current_proc] = quantum + 1;
				
				ptr = qhead[current_prio];
				qhead[current_prio] = ptr->tail;
				if (qhead[current_prio] == NULL)
					qtail[current_prio] = NULL;
				free(ptr);
				
			}
			//Otherwise, rotate processes
			else if (qhead[current_prio] != qtail[current_prio]) {
				ptr = qhead[current_prio];
				qhead[current_prio] = ptr->tail;
				
				qhead[current_prio]->head = NULL;
				ptr->head = qtail[current_prio];
				qtail[current_prio]->tail = ptr;
				ptr->tail = NULL;
				
				qtail[current_prio] = ptr;
			}
			
			current_proc = -1;
		}
		else
			printf("_");
		
		//Check whether any processes arrived this quantum; if so, initialize them and add them to their appropriate queue
		while ((pr[it].arrivaltime < ((float)quantum + 1.0)) && (quantum <= 99)) {
			//Check whether current entry at iterator arrives this quantum
			if ((pr[it].arrivaltime >= (float)quantum)) {
				
				ptr = (NODE *)malloc(sizeof(NODE));
				ptr->pid = it;
				ptr->tail = NULL;
				
				//Case: empty queue
				if (qhead[pr[it].priority - 1] == NULL) {
					qhead[pr[it].priority - 1] = ptr;
					qtail[pr[it].priority - 1] = ptr;
					ptr->head = NULL;
				}
				//Case: tail
				else {
					qtail[pr[it].priority - 1]->tail = ptr;
					ptr->head = qtail[pr[it].priority - 1];
					qtail[pr[it].priority - 1] = ptr;
				}
				
				it++;
			}
			else
				break;
		}
		
		quantum++;
		
		
		//If there's no current process, select one to run next quantum 
		if (current_proc == -1) {
			//Iterate through queues to find a process
			for(j = 0; j < PRIORITY_LIMIT; j++) {
				if (qhead[j] != NULL) {
					current_proc = qhead[j]->pid;
					current_prio = j;
					break;
				}
			}
		}
	}
	
	//Final process metrics: average turnaround, waiting, and response times
	//Initialize per-queue metrics
	for (int v = 0; v < PRIORITY_LIMIT; v++) {
		qturnavg[v] = 0.0;
		qwaitavg[v] = 0.0;
		qresponseavg[v] = 0.0;
	}
	
	//Calculate metrics for each process
	for (int x = 0; x < count; x++) {
		if (runtime[x] > 0) {
			turnaround = (float)end[x] - pr[x].arrivaltime;
			waiting = turnaround - pr[x].runtime;
			response = (float)start[x] - (float)pr[x].arrivaltime;
			turnavg += turnaround;
			waitavg += waiting;
			responseavg += response;
			
			qturnavg[pr[x].priority - 1] += turnaround;
			qwaitavg[pr[x].priority - 1] += waiting;
			qresponseavg[pr[x].priority - 1] += response;
		}
	}
	
	for (int y = 0; y < PRIORITY_LIMIT; y++) {
		qturnavg[y] /= qcount[y];
		qwaitavg[y] /= qcount[y];
		qresponseavg[y] /= qcount[y];
		printf("\n Priority %d average turnaround time: %4.2f \n Priority %d average waiting time: %4.2f \n Priority %d average response time: %4.2f \n Priority %d throughput: %d \n", (y + 1), qturnavg[y], (y + 1), qwaitavg[y], (y + 1), qresponseavg[y], (y + 1), qcount[y]);
		
		output[y + PRIORITY_LIMIT + 1].turnaround = qturnavg[y];
		output[y + PRIORITY_LIMIT + 1].waiting = qwaitavg[y];
		output[y + PRIORITY_LIMIT + 1].response = qresponseavg[y];
		output[y + PRIORITY_LIMIT + 1].throughput = (float)qcount[y];
	}
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\n Overall average turnaround time: %4.2f \n Overall average waiting time: %4.2f \n Overall average response time: %4.2f \n Overall throughput: %d \n", turnavg, waitavg, responseavg, completed);
	
	output[2 * PRIORITY_LIMIT + 1].turnaround = turnavg;
	output[2 * PRIORITY_LIMIT + 1].waiting = waitavg;
	output[2 * PRIORITY_LIMIT + 1].response = responseavg;
	output[2 * PRIORITY_LIMIT + 1].throughput = (float)completed;
	
	return output;
}
