
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "hpf.h"


//Highest priority first simulation function
//Prints runtime and performance of algorithm
void hpf(PROC *pr, int count) {
	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival)
	//Index correlates to process ID (A -> element 0)
	int start[count], end[count];
	
	for (int i = 0; i < count; i++) {
		start[i] = 0;
		end[i] = 0;
	}
	
	
	
	
	printf("\nHighest priority first:\n");
	
	//Main scheduler loop; print process ID for each quantum
	
	// INSERT CODE HERE
	
	
	
	
	//Final process metrics: average turnaround, waiting, and response times
	float turnaround[count], waiting[count], response[count];
	float turnavg, waitavg, responseavg;
	
	//Calculate metrics for each process
	for (int x = 0; x < count; x++) {
		turnaround[i] = end[i] - pr[i].arrivaltime;
		waiting[i] = turnaround[i] - runtime[i];
		response[i] = pr[i].arrivaltime - start[i];
	}
	
	//Average metrics
	for (int y = 0; y < count; y++) {
		turnavg += turnaround[i];
		waitavg += waiting[i];
		responseavg += response[i];
	}
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\nAverage turnaround time: %4.2f \n Average waiting time: %4.2f \n Average response time: %4.2f \n", turnavg, waitavg, responseavg);
	
	//Final algorithm metric: throughput
	
}