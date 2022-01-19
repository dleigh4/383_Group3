
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "sjf.h"


//Shortest job first simulation function
//Prints runtime and performance of algorithm
void sjf(PROC *pr, int count) {
	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival)
	//Index correlates to process ID (A -> element 0)
	int start[count], end[count];
	
	for (int i = 0; i < count; i++) {
		start[i] = 0;
		end[i] = 0;
	}
	
	
	
	
	printf("\nShortest job first:\n");
	
	//Main scheduler loop; print process ID for each quantum
	
	// INSERT CODE HERE
	
	
	
	
	//Final process metrics: average turnaround, waiting, and response times
	float turnaround[count], waiting[count], response[count];
	float turnavg, waitavg, responseavg;
	
	//Calculate metrics for each process
	for (int x = 0; x < count; x++) {
		turnaround[x] = end[x] - pr[x].arrivaltime;
		waiting[x] = turnaround[x] - runtime[x];
		response[x] = pr[x].arrivaltime - start[x];
	}
	
	//Average metrics
	for (int y = 0; y < count; y++) {
		turnavg += turnaround[y];
		waitavg += waiting[y];
		responseavg += response[y];
	}
	
	turnavg /= count;
	waitavg /= count;
	responseavg /= count;
	
	printf("\nAverage turnaround time: %4.2f \n Average waiting time: %4.2f \n Average response time: %4.2f \n", turnavg, waitavg, responseavg);
	
	//Final algorithm metric: throughput
	
}