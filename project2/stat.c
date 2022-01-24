
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "stat.h"


//Initialize stats
void init_stat(STAT *avg) {
	avg->turnaround = 0;
	avg->waiting = 0;
	avg->response = 0;
	avg->throughput = 0;
}


//Increment stats by function output
void incr_stat(STAT *avg, STAT input) {
	avg->turnaround += input.turnaround;
	avg->waiting += input.waiting;
	avg->response += input.response;
	avg->throughput += input.throughput;
}


//Divide stats by factor
void div_stat(STAT *avg, float div) {
	avg->turnaround /= div;
	avg->waiting /= div;
	avg->response /= div;
	avg->throughput /= div;
}


//Print stats
void print_stat(STAT *avg) {
	printf("\n Average turnaround time: %4.2f \n Average waiting time: %4.2f \n Average response time: %4.2f \n Average throughput: %4.2f \n", avg->turnaround, avg->waiting, avg->response, avg->throughput);
}
