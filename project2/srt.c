
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "node.h"
#include "stat.h"
#include "srt.h"


//Shortest remaining time simulation function
//Prints runtime and performance of algorithm
STAT srt(PROC *pr, int count) {
	
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
	
	STAT output;
	output.turnaround = turnavg;
	output.waiting = waitavg;
	output.response = responseavg;
	output.throughput = (float)finished;
	
	return output;
	
}
