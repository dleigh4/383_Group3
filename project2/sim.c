

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "proc.h"
#include "node.h"
#include "stat.h"
#include "fcfs.h"
#include "sjf.h"
#include "srt.h"
#include "rr.h"
#include "hpf.h"

#define PROC_COUNT 30
#define IT_COUNT 5

int main(int argc, char *argv[]){
	
	STAT fcfs_stat, sjf_stat, srt_stat, rr_stat, hpfnp_stat, hpfnpq_stat[PRIORITY_LIMIT], hpfp_stat, hpfpq_stat[PRIORITY_LIMIT], *hpfbuff;
	
	//Track number of statistically usable runs (i.e. throughput > 0)
	int hpfp_usable[PRIORITY_LIMIT];
	int hpfnp_usable[PRIORITY_LIMIT];
	
	//Seeds for process generation
	int seeds[IT_COUNT] = {20, 110, 190, 50, 60};
	
	for (int h = 0; h < PRIORITY_LIMIT; h++) {
		hpfp_usable[h] = 0;
		hpfnp_usable[h] = 0;
	}
	
	init_stat(&fcfs_stat);
	init_stat(&sjf_stat);
	init_stat(&srt_stat);
	init_stat(&rr_stat);
	init_stat(&hpfnp_stat);
	init_stat(&hpfp_stat);
	
	for (int i = 0; i < PRIORITY_LIMIT; i++) {
		init_stat(&(hpfnpq_stat[i]));
		init_stat(&(hpfpq_stat[i]));
	}
	
	for (int j = 0; j < IT_COUNT; j++) {
		//Load processes
		PROC *pr = generateProcs(seeds[j], PROC_COUNT);
		printProcs(pr, PROC_COUNT);
		
		//Run schedulers and increment aggregate stats
		incr_stat(&fcfs_stat, fcfs(pr, PROC_COUNT));
		incr_stat(&sjf_stat, sjf(pr, PROC_COUNT));
		incr_stat(&srt_stat, srt(pr, PROC_COUNT));
		incr_stat(&rr_stat, rr(pr, PROC_COUNT));
		
		//Increment aggregate stats for hpf
		hpfbuff = hpf(pr, PROC_COUNT);
		
		for (int k = 0; k < PRIORITY_LIMIT; k++) {
			if (hpfbuff[k].throughput > 0) {
				hpfnp_usable[k]++;
				incr_stat(&(hpfnpq_stat[k]), hpfbuff[k]);
			}
		}
		
		incr_stat(&hpfnp_stat, hpfbuff[PRIORITY_LIMIT]);
		
		for (int l = 0; l < PRIORITY_LIMIT; l++) {
			if (hpfbuff[l + PRIORITY_LIMIT + 1].throughput > 0) {
				hpfp_usable[l]++;
				incr_stat(&(hpfpq_stat[l]), hpfbuff[l + PRIORITY_LIMIT + 1]);
			}
		}
		
		incr_stat(&hpfp_stat, hpfbuff[2 * PRIORITY_LIMIT + 1]);
		
		free(hpfbuff);
		destroyProcs(pr);
	}
	
	printf("\n\n\n-----------------\nOverall averages:\n-----------------\n");
	printf("\n\nFirst-come first-serve:");
	div_stat(&fcfs_stat, (float)IT_COUNT);
	print_stat(&fcfs_stat);
	printf("\n\nShortest job first:");
	div_stat(&sjf_stat, (float)IT_COUNT);
	print_stat(&sjf_stat);
	printf("\n\nShortest remaining time:");
	div_stat(&srt_stat, (float)IT_COUNT);
	print_stat(&srt_stat);
	printf("\n\nRound robin:");
	div_stat(&rr_stat, (float)IT_COUNT);
	print_stat(&rr_stat);
	
	printf("\n\nHighest priority first (non-preemptive):");
	for (int m = 0; m < PRIORITY_LIMIT; m++) {
		printf("\nPriority %d", m + 1);
		div_stat(&(hpfnpq_stat[m]), (float)hpfnp_usable[m]);
		print_stat(&(hpfnpq_stat[m]));
	}
	printf("\nOverall:");
	div_stat(&hpfnp_stat, (float)IT_COUNT);
	print_stat(&hpfnp_stat);
	
	printf("\n\nHighest priority first (preemptive):");
	for (int n = 0; n < PRIORITY_LIMIT; n++) {
		div_stat(&(hpfpq_stat[n]), (float)hpfp_usable[n]);
		print_stat(&(hpfpq_stat[n]));
	}
	printf("\nOverall:");
	div_stat(&hpfp_stat, (float)IT_COUNT);
	print_stat(&hpfp_stat);
	
	
	return 0;	
}




