#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "utility.h"

struct job {
	char char_id;	//Process name
	int num_id;		//Process index in job array (for clarity)
	int size;		//Memory size (in page counts of 5, 11, 17, or 31)
	float arrival;	//Arrival time (in deciseconds/100 milliseconds)
	int duration;	//Duration (in deciseconds/100 milliseconds)
	int *accesses;	//Page references (has length of duration)
} typedef JOB;

struct page {
	int job_id		//Process index in job array (for identification/access)
	int page_num	//Page index in process (for identification/access)
	int counter		//General-purpose counter for storing access information (frequency or recency)
	int mem_loc		//Location in memory map
} typedef PAGE

//Page replacement array variables
PAGE replacement_arr[100];
char mem_map[101];				//Memory map -- initialized to all periods at start of sim (other than terminating character)
int start;						//Only practically used in FIFO; initialized to 0 in all sims anyway
int count;
int max;						//Used in lru?



//Given a process's latest page access and its total size, return the next reference
int generate_ref(int index, int pagecount);

//Return a valid service duration (1, 2, 3, 4, or 5)
int generate_dur(void);

//Float comparison function for sorting (via library qsort function)
int flt_cmp(const void *a, const void *b);



/*	Replacement algorithms, called on all page requests regardless of presence in memory

	Takes the numerical process id, page number of the current access, and character process id
	
	Returns a PAGE with job_id -1 if no replacement needs to be made, given page is already in memory
		(initializing the new entry)
	Returns a PAGE with job_id -5 if no replacement needs to be made, given there's room to insert
		(in which case its counter/the array's counters is/are updated as needed)
	Returns the PAGE that was replaced if a replacement needs to be made (initializing the new entry)
	
	Assume the main program itself checks whether there's room to insert an entirely new process; it calls
		the function if so, and will print the operation output based on the returned result
	Should use the page replacement array variables directly (as globals); assume they're initialized at
		starting values upon every new simulation; main sim will use count to check whether there's room
		to start a new job (based on the 4 page/exiting process limit)
	If replacing/inserting, need to update memory map entry*/
PAGE fifo_rep(int id, int pagenum, char sym);
PAGE lru_rep(int id, int pagenum, char sym);
PAGE lfu_rep(int id, int pagenum, char sym);
PAGE mfu_rep(int id, int pagenum, char sym);
PAGE rand_rep(int id, int pagenum, char sym);



//Page array utility functions (ONLY FOR TRACKING REPLACEMENT METRICS, NOT ACTUAL DATA LOCATIONS)

//	Initializer; given pointer to page and values to use, initialize the page's values
void init_page(PAGE *page, int id, int pagenum, int counter, int mem_loc);

//	Comparison; given process id and page number, return whether input PAGE is equivalent
int page_check(int id, int pagenum, PAGE page);

//	Array search; given process id and page number, return the array index it was found in or return -1 if not found
//		(based on page replacement array variables replacement_arr, start, and count)
int page_search(int id, int pagenum);

//	Counter comparison function for sorting (via library qsort function)
int pgc_cmp(const void *a, const void *b);

//	Clear process data; given a process's conclusion, remove all of its array entries and decrement count for each
void clear_proc(int id);



int main(int argc, char **argv) {
	
	//Initialize output arrays
	
	
	mem_map[100] = '\0';
	
	//Generate workloads
	JOB min_jobs[6][150];
	float arrivals[150];
	char id;
	
	for (int i = 0; i < 6; i++) {
		
		id = '!';
		
		for (int j = 0; j < 150; j++) {
			
			//Don't use '.' or DEL as a process id
			if (id == '.' || id == 127)
				id++;
			
			//Generate job information independently from arrival time (to be sorted and assigned later)
			min_jobs[i][j].char_id = id++;
			min_jobs[i][j].num_id = j;
			
			switch(rand() % 4) {
				case 0:
					min_jobs[i][j].size = 5;
					break;
				case 1:
					min_jobs[i][j].size = 11;
					break;
				case 2:
					min_jobs[i][j].size = 17;
					break;
				default:
					min_jobs[i][j].size = 31;
			}
			
			min_jobs[i][j].duration = generate_dur();
			min_jobs[i][j].accesses = (int *)malloc(sizeof(int) * min_jobs[i][j].duration);
			min_jobs[i][j].accesses[0] = rand() % min_jobs[i][j].size;
			
			for (int k = 1; k < min_jobs[i][j].duration; k++) 
				min_jobs[i][j].accesses[k] = generate_ref(min_jobs[i][j].accesses[k - 1], min_jobs[i][j].size);
			
			arrivals[j] = (((float) * rand()) / ((float) * RAND_MAX)) * 600.0;
		}
		
		//Sort arrival times and assign to job list
		qsort(arrivals, 150, sizeof(int), flt_cmp);
		for (int l = 0; l < 150; l++) 
			min_jobs[i][l].arrival = arrival[l];
		
		
	}
	
	
	//Run loop for each replacement algorithm
	for (int m = 0; m < 5; m++) {
		
		//Re-initializing data structures for this run
		start = 0;
		count = 0;
		max = 0;
		
		for (int n = 0; n < 100; n++) 
			mem_map[n] = '.';
		
		//Run minute-based sim 5 times
		
			//Re-initialize memory map (only locations 0-99, where 100 is the terminating char)
			
			
			//Main loop
			
				//Check job list;
			
			
		//Run reference-based sim
		
			
			
	
	}
	
	//Print output
	
	//Free workload page reference arrays
}



int flt_cmp(const void *a, const void *b) {
	return ( *(float *)a > *(float *)b );
}



int generate_ref(int index, int pagecount) {
	if ((rand() % 10) > 2) 
		return index + (rand() % 3) - 1;
	else
		return (rand() % (pagecount - 3) + index + 2) % pagecount;
}



int generate_dur(void) {
	return ((rand() % 5) + 1) * 10;
}



void init_page(PAGE *page, int id, int pagenum, int counter, int mem_loc) {
	page->job_id = id;
	page->page_num = pagenum;
	page->counter = counter;
	page->mem_loc = mem_loc;
}



int page_check(int id, int pagenum, PAGE page) {
	if (page->job_id == id && page->page_num == pagenum) {
		return 1;
	} else
		return -1;
}



int page_search(int id, int pagenum) {
	for (int i = start; i < start + count; i++) {
		if page_check(id, pagenum, replacement_arr[i % 100])
			return i;
	}
	
	return -1;
}



int pgc_cmp(const void *a, const void *b) {		//Takes pointers to PAGEs
	return ( ((PAGE *)a)->counter - ((PAGE *)b)->counter );
}



void clear_proc(int id) {
	for (int i = start; i < count; i++) {
		if (replacement_arr[i % 100].job_id == id) {
			mem_map[replacement_arr[i % 100].mem_loc] = '.';
			
			remaining = count - i;
			
			for (int j = i; j < remaining; j++) { //Shift array
				init_page(&replacement_arr[j % 100], replacement_arr[(j + 1) % 100].job_id, replacement_arr[(j + 1) % 100].page_num, replacement_arr[(j + 1) % 100].counter, replacement_arr[(j + 1) % 100].mem_loc);
			}
			
			count--;
			i--;
		}
	}
}



PAGE fifo_rep(int id, int pagenum, char sym) {	//Maintaining the replacement array as a circular queue
												//Start of array at start, end is start + count - 1
	PAGE output;								//Indexed in no particular order
	int index = page_search(id, pagenum);
	int mem_loc;
	
	if (index > -1) {							//Case: page in array already
		output.job_id = -1;
	} else if (count < 100) {					//Case: room to insert
		output.job_id = -5;
		for (mem_loc = 0; i < 100; i++) {			//	Look for open location in memory map
			if (mem_map[i] == '.') {
				mem_map[i] = sym;
				break;
			}
		}
		init_page(&(replacement_arr[(start + count - 1) % 100]), id, pagenum, mem_loc);
		count++;
	} else {									//Case: need to replace
		index = start;								//	Swap first (first index) with new page
		mem_loc = replacement_arr[index].mem_loc;
		init_page(&output, replacement_arr[index].job_id, replacement_arr[index].page_num, 0, 0);
		init_page(&replacement_arr[index], id, pagenum, 0, mem_loc);
		mem_map[mem_loc] = sym;					//	Rewrite location in memory map
		start++;								//	Shift start of queue
	}
	
	return output;
}



PAGE rand_rep(int id, int pagenum, char sym) {	//Start of array is always at index 0, end is count - 1
	
	PAGE output;
	int index;
	int mem_loc;
	
	if (page_search(id, pagenum) > -1) {		//Case: page in array already
		output.job_id = -1;
		
	} else if (count < 100) {					//Case: room to insert
		output.job_id = -5;
		for (mem_loc = 0; i < 100; i++) {			//	Look for open location in memory map
			if (mem_map[i] == '.') {
				mem_map[i] = sym;
				break;
			}
		}
		init_page(&(replacement_arr[count - 1]), id, pagenum, mem_loc);
		count++;
	} else {									//Case: need to replace
		index = rand() % count;
		mem_loc = replacement_arr[index].mem_loc;
		init_page(&output, replacement_arr[index].job_id, replacement_arr[index].page_num, 0, 0);
		init_page(&replacement_arr[index], id, pagenum, 0, mem_loc);
		mem_map[mem_loc] = sym;					//	Rewrite location in memory map
	}
	
	return output;
}



PAGE lfu_rep(int id, int pagenum, char sym) {	//Sorting array by counter value on each access
												//Start of array is always at index 0, end is count - 1
	PAGE output;								//Indexed in non-decreasing order
	int index = page_search(id, pagenum);
	int mem_loc;
	
	if (index > -1) {							//Case: page in array already
		replacement_arr[index].counter++;		//	Increment page's counter
		output.job_id = -1;
	} else if (count < 100) {					//Case: room to insert
		output.job_id = -5;
		for (mem_loc = 0; i < 100; i++) {			//	Look for open location in memory map
			if (mem_map[i] == '.') {
				mem_map[i] = sym;
				break;
			}
		}
		init_page(&(replacement_arr[count - 1]), id, pagenum, mem_loc);
		count++;
	} else {									//Case: need to replace
		index = 0;								//	Swap LFU (first index) with new page
		mem_loc = replacement_arr[index].mem_loc;
		init_page(&output, replacement_arr[index].job_id, replacement_arr[index].page_num, 0, 0);
		init_page(&replacement_arr[index], id, pagenum, 0, mem_loc);
		mem_map[mem_loc] = sym;					//	Rewrite location in memory map
	}
	
	qsort(replacement_arr, count, sizeof(PAGE), pgc_cmp);		//Sort array
	
	return output;
}



//I recommend modifying LFU for the other algorithms
//	FIFO needs to have wraparound implemented; clear_proc and page_search should support it already
//	LRU probably needs to keep track of the array max so that the newest access always has the highest relative value
//		(use max, declared at the top with the other variables)
//	MFU just needs to change which index is used in the replacement case


































