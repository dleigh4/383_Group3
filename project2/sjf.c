
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "node.h"
#include "stat.h"
#include "sjf.h"


//Shortest job first simulation function
//Prints runtime and performance of algorithm
STAT sjf(PROC *pr, int count) {
	
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
	
	
	
	
	printf("\nShortest job first:\n");
	
	//Main scheduler loop; print process ID for each quantum
	while ((finished < count)) {
		
		//Run process at head of queue, if any
		if (qcount > 0) {
		
			//Mark start of execution if new process
			if (runtime[queue[0]] == 0)
				start[queue[0]] = current_quantum;
			
			printf("%c", pr[queue[0]].name);
			runtime[queue[0]]++;
			
			//Check for completion
			if ((float)runtime[queue[0]] >= pr[queue[0]].runtime) {
			
				end[queue[0]] = current_quantum + 1;
				qcount--;
				for(j = 0; j < qcount; j++)
					queue[j] = queue[j+1];
				
				finished++;
				
				//If past quantum 99, free rest of queue
				if (current_quantum > 99) {
					qcount = 0;
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
				
				queue[qcount] = i;
				start[i] = current_quantum;
				
				//Case: nonempty queue (not including current process)
				if (qcount > 1) {
					
					j = qcount - 1;
					
					while ((j > 0) && (pr[queue[j]].runtime > pr[i].runtime)) {
						queue[j + 1] = queue[j % count];
						queue[j % count] = i;
						j--;
					}
				
				}
				count++;
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
	
	STAT output;
	output.turnaround = turnavg;
	output.waiting = waitavg;
	output.response = responseavg;
	output.throughput = (float)finished;
	
	return output;
	
}
