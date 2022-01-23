
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "node.h"
#include "hpf.h"



//Highest priority first simulation function (both versions)
//Prints runtime and performance of algorithm
void hpf(PROC *pr, int count) {
	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival) as well as total processes completed
	//Index correlates to process ID (A -> element 0)
	int start[count], end[count], completed, quantum, prio;
	int age[count];
	int runtime[count];
	NODE *ptr, *ptr2, *tail;
	
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
	int i, j, k, l, m, n, current_prio;
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
	
	
	for (int y = 0; y < PRIORITY_LIMIT; y++) {
		qturnavg[y] /= qcount[y];
		qwaitavg[y] /= qcount[y];
		qresponseavg[y] /= qcount[y];
		printf("\n Priority %d average turnaround time: %4.2f \n Priority %d average waiting time: %4.2f \n Priority %d average response time: %4.2f \n Priority %d throughput: %d \n", (y + 1), qturnavg[y], (y + 1), qwaitavg[y], (y + 1), qresponseavg[y], (y + 1), qcount[y]);
	}
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\n Overall average turnaround time: %4.2f \n Overall average waiting time: %4.2f \n Overall average response time: %4.2f \n Overall throughput: %d \n", turnavg, waitavg, responseavg, completed);
	
	
	
	
	
	
	
	
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
				ptr->head = tail;
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
	}
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\n Overall average turnaround time: %4.2f \n Overall average waiting time: %4.2f \n Overall average response time: %4.2f \n Overall throughput: %d \n", turnavg, waitavg, responseavg, completed);
	
}
/*	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival) as well as total processes completed
	//Index correlates to process ID (A -> element 0)
	int start[count], end[count], completed, quantum, prio;
	NODE *ptr, *ptr2, *tail;
	
	for (int i = 0; i < count; i++) {
		start[i] = -1;
		end[i] = 0;
	}
	
	//NON-PREEMPTIVE
	printf("\nHighest priority first (non-preemptive):\n");
	
	//Schedule queues: track performance across each as well as number of processes queued
	NODE *qhead[PRIORITY_LIMIT];
	NODE *qtail[PRIORITY_LIMIT];
	
	for (int j = 0; j < PRIORITY_LIMIT; j++) {
		qhead[j] = NULL;
		qtail[j] = NULL;
	}
	
	//Aging and runtime: track time in current queue and overall execution time per-process
	int age[count];
	int runtime[count];
	int current_proc = -1;
	int m, n, curr_prio;
	
	//Main scheduler loop; print process ID for each quantum
	completed = 0;
	quantum = 0;
	
	while (completed < count) {
		
		
		
		//Check whether any processes arrived this quantum; if so, initialize them and add them to their appropriate queue
		for(int k = 0; k < count; k++) {
			if ( (pr[k].arrivaltime > (float)quantum) && (pr[k].arrivaltime < (float)(quantum + 1))) {
			
				//printf(" inserting:%d ", k);
				
				prio = pr[k].priority - 1;
				
				//Linked list ops: if the queue is empty, (re-)initialize; otherwise, insert at current end
				if (qhead[prio] == NULL) {
					qhead[prio] = (NODE *)malloc(sizeof(NODE));
					qtail[prio] = qhead[prio];
					qhead[prio]->head = qhead[prio]->head;
					qhead[prio]->tail = qhead[prio]->head;
					qhead[prio]->pid = k;
				}
				else {
					qtail[prio]->tail = (NODE *)malloc(sizeof(NODE));	//Prev tail->tail points to tail
					qhead[prio]->head = qtail[prio]->tail;				//Head->head points to tail
					qtail[prio]->tail->head = qtail[prio];				//Tail->head points to old tail
					qtail[prio]->tail->tail = qhead[prio];				//Tail->tail points to head
					qtail[prio] = qtail[prio]->tail;					//Tail pointer points to tail
					qtail[prio]->pid = k;
				}
				
				age[k] = 0;
				runtime[k] = 0;
			}
		}
		
		
		
		//Check whether any processes aged out this quantum (excluding queue 1); if so, move them up and reset their counter
		for(int l = 1; l < PRIORITY_LIMIT; l++) {
			
			ptr = qhead[l];
			tail = qtail[l];
			
			//If empty, go to next 
			if (ptr == NULL)
				continue;
				
			//Traverse queue (manually inspecting tail to cover single element/tail queue case)
			while (ptr != tail) {
				printf("\nsearching %d %d %d %d", l, ptr, ptr->pid, tail->pid);
				//If of age, reset and move up
				if (age[ptr->pid] >= 5) {
					printf(" aging up %d with prio %d\n", ptr->pid, pr[ptr->pid].priority);
					age[ptr->pid] = 0;
					ptr->head->tail = ptr->tail;
					ptr->tail->head = ptr->head;
					
					//If head, set to new head
					if (ptr == qhead[l]) {
						printf("head");
						qhead[l] = ptr->tail;
					}
					
					//If new queue is empty, initialize
					if (qhead[l-1] == NULL) {
						printf("empty");
						qhead[l-1] = ptr;
						qtail[l-1] = ptr;
						ptr->head = ptr;
						ptr->tail = ptr;
					}
					//Otherwise, insert at tail
					else {
						printf("tail");
						qtail[l-1]->tail = ptr;
						qhead[l-1]->head = ptr;
						ptr->head = qtail[l-1];
						ptr->tail = qhead[l-1];
						qtail[l-1] = ptr;
					}
				}
				//Go to next entry if not of age
				else {
					age[ptr->pid]++;
					ptr = ptr->tail;
				}
			}
			
			//Tail cases: 
			//Not of age
			if (age[ptr->pid] < 5) {
				age[ptr->pid]++;
				continue;
			}
			//Current process:
			if (current_proc == ptr->pid)
				continue;
			
			//printf(" aging up tail %d \n", ptr->pid);
			//Of age: reset and move
			age[ptr->pid] = 0;
			
			//Single element
			if (qtail[l] == qhead[l]) {
				qtail[l] = NULL;
				qhead[l] = NULL;
			}
			//Tail
			else {
				ptr->head->tail = ptr->tail;
				ptr->tail->head = ptr->head;
				qtail[l] = ptr->head;
			}
			
			//If new queue is empty, initialize
			if (qhead[l-1] == NULL) {
				qhead[l-1] = ptr;
				qtail[l-1] = ptr;
				ptr->head = ptr;
				ptr->tail = ptr;
			}
			//Otherwise, insert at tail
			else {
				qtail[l-1]->tail = ptr;
				qhead[l-1]->head = ptr;
				ptr->head = qtail[l-1];
				ptr->tail = qhead[l-1];
				qtail[l-1] = ptr;
			}
			
		}
		
		
		
		//Run process
		if (current_proc != -1) {
			printf("%c", pr[current_proc].name);
			
			//Remove from queue if complete
			if( (float)runtime[current_proc] >= pr[current_proc].runtime - 1 ) {
				
				ptr = qhead[curr_prio];
				
				//Multiple queue entries: replace head
				if (qhead[m] != qtail[curr_prio]) {
					//printf(" nonempty queue ");
					ptr->head->tail = ptr->tail;
					ptr->tail->head = ptr->head;
					qhead[curr_prio] = ptr->tail;
				}
				//One entry: empty queue
				else {
					//printf(" emptying queue ");
					qhead[curr_prio] = NULL;
					qtail[curr_prio] = NULL;
				}
				
				free(ptr);
				
				completed++;
				
				//If past quantum 99, free rest of queues
				if (quantum > 99) {
					for (n = 0; n < PRIORITY_LIMIT; n++) {
						ptr = qhead[n];
						while (ptr != NULL) {
							ptr2 = ptr->tail;
							free(ptr);
							ptr = ptr2;
						}
					}
					break;
				
				//printf(" completed:%d %d %d\n", current_proc, quantum, completed);
				end[current_proc] = quantum + 1;
				age[current_proc] = 0;
				current_proc = -1;
			}
			else {
				//If not, increment runtime
				runtime[current_proc]++;
				age[current_proc] = 0;
			}
		} 
		else {
			printf("-%d-", quantum);
		}
		
		//If there's no current process, select one to run next quantum 
		if (current_proc == -1){
			
			//Iterate through queues to find a process
			for (m = 0; m < PRIORITY_LIMIT; m++) {
			
				if (qhead[m] != NULL) {
					current_proc = qhead[m]->pid;
					curr_prio = m;
					//printf(" started:%d %d %d\n", current_proc, quantum, m);
					
					if (runtime[current_proc] == 0) {
						start[current_proc] = quantum + 1;
					}
					
					break;
				}
			}
		
		}
		}
		
		quantum++;
		
		//printf(" (COMPLETE - %d) ", completed);
			

	}
	
	
	//Final process metrics: average turnaround, waiting, and response times
	float turnaround, waiting, response;
	float turnavg, waitavg, responseavg;
	
	float qturnavg[PRIORITY_LIMIT], qwaitavg[PRIORITY_LIMIT], qresponseavg[PRIORITY_LIMIT];
	int qcount[PRIORITY_LIMIT];
	
	//Initialize per-queue metrics
	for (int v = 0; v < PRIORITY_LIMIT; v++) {
		qturnavg[v] = 0.0;
		qwaitavg[v] = 0.0;
		qresponseavg[v] = 0.0;
		qcount[v] = 0.0;
	}
	
	//Calculate metrics for each process
	for (int x = 0; x < count; x++) {
		printf("\n%d start:%d end:%d", x, start[x], end[x]);
		turnaround = end[x] - pr[x].arrivaltime;
		waiting = turnaround - pr[x].runtime;
		response = pr[x].arrivaltime - start[x];
		turnavg += turnaround;
		waitavg += waiting;
		responseavg += response;
		
		qturnavg[pr[x].priority] += turnaround;
		qwaitavg[pr[x].priority] += waiting;
		qresponseavg[pr[x].priority] += response;
		qcount[pr[x].priority]++;
	}
	
	
	for (int y = 0; y < PRIORITY_LIMIT; y++) {
		qturnavg[y] /= qcount[y];
		qwaitavg[y] /= qcount[y];
		qresponseavg[y] /= qcount[y];
		printf("\n Priority %d average turnaround time: %4.2f \n Priority %d average waiting time: %4.2f \n Priority %d average response time: %4.2f \n", (y + 1), qturnavg[y], (y + 1), qwaitavg[y], (y + 1), qresponseavg[y]);
	}
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\n Overall average turnaround time: %4.2f \n Overall average waiting time: %4.2f \n Overall average response time: %4.2f \n", turnavg, waitavg, responseavg);
	
	//Final algorithm metric: throughput
	
} */
