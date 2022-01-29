#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <ctype.h>

struct sell_arg {
	char type;
	int buycount;
	int *buyers;
	char **seats;		//Array of strings - assignment denoted by having assigned string rather than NULL
	int *min;
} typedef SELLARG;

pthread_cond_t start_cond = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER; 


// seller thread to serve one time slice (1 minute) 
void *sell(SELLARG *args) 
{ 
	char selltype = args->type;
	int curr_cust = 0;
	int cust_total = args->buycount;
	int *ready = args->min;				//External counter for number of sellers ready to move on to next minute
	int time = 0;						//Internal counter for current minute
	int *customers = args->buyers;

    while (having more work todo) 
    { 
     pthread_mutex_lock(&start_mutex); 
     pthread_cond_wait(&start_cond, &start_mutex); 
     pthread_mutex_unlock(&start_mutex); 
	 
     // Serve any buyer available in this seller queue that is ready  
     // now to buy ticket till done with all relevant buyers in their queue              
             
     } 
     return NULL;  // thread exits 
} 



void wakeup_all_seller_threads() 
{ 
    pthread_mutex_lock(&start_mutex); 
    pthread_cond_broadcast(&start_cond); 
    pthread_mutex_unlock(&start_mutex); 
} 



int cmp (const void *p1, const void *p2) {
	return *((int *)p1) - *((int *)*p2);
}



int main(int argc, char *argv[]){
	
	int i, n; 
	SELLARG args[10];
    pthread_t   tids[10]; 
	
	
    // Create necessary data structures for the simulator.  
	char *seats[100];
	for (int j = 0; j < 100; j++) 
		seats[j] = NULL;
	
	
	
    // Create buyers list for each seller ticket queue based on the  
    // N value within an hour and have them in the seller queue. 
	n = atoi(argv[1]);
	int cust[10][n];
	for (int k = 0; k < 10; k++) {
		
		for (int l = 0; l < 10; l++) {
			cust[k][l] = rand() % 60;
		}
		
		args[k].buycount = n;
		args[k].buyers = cust[k];
		args[k].seats = seats;
		
		qsort(cust[k], n, sizeof(int), cmp);
	}
	
  
    // Create 10 threads representing the 10 sellers. 
    args[0].type = “H”; 
    pthread_create(&tids[0], NULL, sell, &args[0]); 
 
    for (i = 1; i < 4; i++) {
		args[i].type = "M";
        pthread_create(&tids[i], NULL, sell, &args[i]); 
	}
  
    for (i = 4; i < 10; i++) {
		args[i].type = "L";
        pthread_create(&tids[i], NULL, sell, &args[i]); 
	}
 
    // wakeup all seller threads 
    wakeup_all_seller_threads(); 
    
    // wait for all seller threads to exit 
    for (i = 0 ; i < 10 ; i++) 
     pthread_join(&tids[i], NULL); 
 
    // Printout simulation results 
 
    exit(0);
	
	return 0;	
}




