
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"
#include "fcfs.h"

//Function to count Max value
int max(int num1, int num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

//First-come first-served simulation function
//Prints runtime and performance of algorithm
void fcfs(PROC *pr, int count) {
	
	//Metric arrays: track starting and completion time of each process's execution (not from arrival)
	//Index correlates to process ID (A -> element 0)

	printf("\nFirst-come first-served:\n");

	int turnaround_time = 0;
    	int waiting_time = 0;
	int response_time = 0;
	
	//Arrays to store the metrics
	float turnaround[count], waiting[count], response[count];
    	float turnaround_avg, waiting_avg, response_avg;
	

	int start_time = 0;
	int end_time = 0;

	for (int i = 0; i < count; i++) {
	
        start_time  = max(pr[i].arrivaltime, end_time);
        end_time = start_time + pr[i].runtime;

        if (start_time > 100) {
            break;
        }
        
        
	//Calculate TAT, WT, RT for each process and store it in respective arrays
	turnaround[i] = end_time - pr[i].arrivaltime;
        waiting[i] = start_time - pr[i].arrivaltime;
        response[i] = end_time - start_time; 
    //Printing just for debugging
	printf("-------------------------\n");  
	printf("START TIME %d\n",start_time);
	printf("END TIME %d\n",end_time);
	printf("%lf\n",turnaround[i]);
	printf("%lf\n",waiting[i]);
	printf("%lf\n",response[i]);
	printf("-------------------------"); 


	//Calculating toatl metrics to calcualte avgs metrics (incrementing process by process)
        turnaround_time += end_time - pr[i].arrivaltime;
        waiting_time += start_time - pr[i].arrivaltime;
    	response_time += end_time - start_time;
	
	}
	
	turnaround_avg = turnaround_time / (double)count;
    	waiting_avg = waiting_time / (double)count;
    	response_avg = response_time / (double)count;
	
	//Printing the metrics
	printf("------> FCFS <------\n");
    	printf("Average Turnaround Time: %lf \n", turnaround_avg);
    	printf("Waiting Time Average: %lf \n", waiting_avg);
    	printf("Average Response Time: %lf \n", response_avg);
}




