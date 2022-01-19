

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "proc.h"
#include "fcfs.h"
#include "sjf.h"
#include "srt.h"
#include "rr.h"
#include "hpf.h"

#define PROC_COUNT 10
#define RAND_SEED 0

int main(int argc, char *argv[]){
	
	//Load processes
	PROC *pr = generateProcs(RAND_SEED, PROC_COUNT);
	printProcs(pr, PROC_COUNT);
	
	//Run schedulers
	fcfs(pr, PROC_COUNT);
	sjf(pr, PROC_COUNT);
	srt(pr, PROC_COUNT);
	rr(pr, PROC_COUNT);
	hpf(pr, PROC_COUNT);
	
	destroyProcs(pr);
	
	return 0;	
}




