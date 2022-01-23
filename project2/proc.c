
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "proc.h"


//Float comparison wrapper function
int fltcmp (const void *a, const void *b) {
	return (*(float *)a > *(float *)b);
}



//Process generation function; includes sorting
//Returns an array of *count* PROCs sorted in arrival order
PROC *generateProcs(int seed, int count) {
	
	//Allocate array
	PROC *procarray = (PROC *)(malloc(sizeof(PROC) * count));
	
	srand(seed);
	float arrivalarray[count];
	char name = 'A';
	
	//Generate arrival times
	for (int h = 0; h < count; h++) {
		arrivalarray[h] = (float)rand() / (float)(RAND_MAX / ARRIVAL_LIMIT);
	}
	
	//Sort arrival times
	qsort(arrivalarray, count, sizeof(float), fltcmp);
	
	//Assign names, runtimes, arrival times, and priorities
	for (int i = 0; i < count; i++){
		procarray[i].name = name++;
		if (name == '[')
			name = 'a';
		procarray[i].arrivaltime = arrivalarray[i];
		procarray[i].runtime = (float)rand() / (float)(RAND_MAX / RUNTIME_LIMIT);
		procarray[i].priority = (rand() % PRIORITY_LIMIT) + 1;
		
		//Ensure runtime is above lower boundary
		if (procarray[i].runtime == 0)
			procarray[i].runtime += 0.1;
	}
	
	return procarray;
}




//Process printing function
//Prints processes and immediate attributes
void printProcs(PROC *pr, int count) {
	
	printf("Name\tArrival\tRuntime\tPriority\n");
	for (int i = 0; i < count; i++) 
		printf("%c\t%2.2f\t%2.2f\t%d\n", pr[i].name, pr[i].arrivaltime, pr[i].runtime, pr[i].priority);
}




//Process destroying function
//Frees a process array
void destroyProcs(PROC *pr) {
	free(pr);
}
	
		






