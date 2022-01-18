

# ifndef PROC_H
# define PROC_H

typedef struct proc PROC;


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
