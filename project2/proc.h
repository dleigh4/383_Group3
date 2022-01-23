

# ifndef PROC_H
# define PROC_H

#define ARRIVAL_LIMIT 99.0
#define RUNTIME_LIMIT 10.0
#define PRIORITY_LIMIT 4


//Proc struct
struct proc {
	char name;				//Alphabetical, starting from A
	float arrivaltime;		//From 0 to 99 quanta
	float runtime;			//From 0.1 to 10 quanta
	int priority;			//1 is highest priority; 4 is lowest
} typedef PROC;

//Process generation function; includes sorting
//Returns an array of *count* PROCs sorted in arrival order
PROC *generateProcs(int seed, int count);


//Process printing function
//Prints processes and immediate attributes
void printProcs(PROC *pr, int count);


//Process destroying function
//Frees a process array
void destroyProcs(PROC *pr);

# endif
