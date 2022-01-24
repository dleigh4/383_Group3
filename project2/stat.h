

# ifndef STAT_H
# define STAT_H

//Storing function outputs in struct
struct stat {
	float turnaround;
	float waiting;
	float response;
	float throughput;
} typedef STAT;

//Initialize stats
void init_stat(STAT *avg);

//Increment stats by function output
void incr_stat(STAT *avg, STAT input);

//Divide stats by factor
void div_stat(STAT *avg, float div);

//Print stats
void print_stat(STAT *avg);

# endif
