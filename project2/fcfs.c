
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "fcfs.h"

//Function to count Max value
float max(float num1, float num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

//First-come first-served simulation function
//Prints runtime and performance of algorithm
void fcfs(PROC *pr, int count) {
	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival)
	//Index correlates to process ID (A -> element 0)

	printf("\nFirst-come first-served:\n");

	float turnaround_time = 0;
    	float waiting_time = 0;
	float response_time = 0;
	
	//Arrays to store the metrics
	float turnaround[count], waiting[count], response[count];
    	float turnaround_avg, waiting_avg, response_avg;
	

	float start_time = 0;
	float end_time = 0;
	printf("\n-----------Process details--------------\n");
	for (int i = 0; i < count; i++) {
        start_time  = max(pr[i].arrivaltime, end_time); 
        end_time = start_time + pr[i].runtime;
	printf("Process name: %c, Arrival time: %f, start time: %f, end time: %f\n", pr[i].name, pr[i].arrivaltime, start_time, end_time); 
        if (start_time > 100) {
            break;
        }
        
        
	//Calculate TAT, WT, RT for each process and store it in respective arrays
	//turnaround[i] = end_time - pr[i].arrivaltime;
    //waiting[i] = start_time - pr[i].arrivaltime;
    //response[i] = end_time - start_time; 


	//Calculating toatl metrics to calcualte avgs metrics (incrementing process by process)
	turnaround_time += end_time - pr[i].arrivaltime;
	waiting_time += start_time - pr[i].arrivaltime;
	response_time += end_time - start_time;
	
	}
	
	turnaround_avg = turnaround_time / (double)count;
	waiting_avg = waiting_time / (double)count;
	response_avg = response_time / (double)count;
	
	
	printf("---------------------------------------\n");
	printf("FIRST COME FIRST SERVE");
	printf("\n---------------------------------------\n");
	//Printing the metrics
	printf("Average Turnaround Time: %lf \n", turnaround_avg);
	printf("Waiting Time Average: %lf \n", waiting_avg);
	printf("Average Response Time: %lf \n", response_avg);
	
	printf("---------------------------------------\n");
}



