

# ifndef NODE_H
# define NODE_H

#include "proc.h"

//Using linked lists to implement queues
struct node {
	int pid;				//Process ID; maps A -> 0
	struct node *head;
	struct node *tail;
} typedef NODE;



# endif
