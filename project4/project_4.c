#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
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
	int job_id;		//Process index in job array (for identification/access)
	int page_num;	//Page index in process (for identification/access)
	int counter;		//General-purpose counter for storing access information (frequency or recency)
	int mem_loc;		//Location in memory map
} typedef PAGE;

struct ref {
	int job_id;
	int index;
	float time;
	int last;
} typedef REF;

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

//Reference comparison function for sorting (via linked list sort function)
int ref_cmp(void *a, void *b);


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

PAGE (*rep[5]) (int id, int pagenum, char sym);

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
void clear_proc(int id, char char_id);



int main(int argc, char **argv) {

	freopen("output.txt", "w", stdout);

	srand(0);
	
	//Initialize output arrays
	float proc_count[5];
	int page_hit[5];
	int page_miss[5];
	
	for (int h; h < 5; h++) {
		proc_count[h] = 0;
		page_hit[h] = 0;
		page_miss[h] = 0;
	}
	
	mem_map[100] = '\0';
	float time;
	int finished;
	PAGE op_out;
	
	//Generate workloads
	JOB min_jobs[6][150];
	float arrivals[150];
	char id;
	int proc_it;
	int proc_fl;
	linked_list *refs;
	REF *pr;
	REF pr_value;
	float offset;
	
	rep[0] = fifo_rep;
	rep[1] = lru_rep;
	rep[2] = lfu_rep;
	rep[3] = mfu_rep;
	rep[4] = rand_rep;
	
	for (int i = 0; i < 6; i++) {
		
		id = '!';
		
		for (int j = 0; j < 150; j++) {
		
			//Rollover if id becomes a non-ascii designation or DEL
			if (id >= 127)
				id = '!';
			
			//Don't use '.'as a process id
			if (id == '.')
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
			
			arrivals[j] = (((float)rand()) / ((float)RAND_MAX)) * 600.0;
		}
		
		//Sort arrival times and assign to job list
		qsort(arrivals, 150, sizeof(int), flt_cmp);
		for (int l = 0; l < 150; l++) 
			min_jobs[i][l].arrival = arrivals[l];
		
		
	}
	
	//Run loop for each replacement algorithm
	for (int m = 0; m < 5; m++) {
			
		switch(m) {
			case 0:
				printf("\n\n\nFIFO\n");
				break;
			case 1:
				printf("\n\n\nLRU\n");
				break;
			case 2:
				printf("\n\n\nLFU\n");
				break;
			case 3:
				printf("\n\n\nMFU\n");
				break;
			case 4:
				printf("\n\n\nRAND\n");
				break;
		}
		
		//Run minute-based sim 5 times
		for (int n = 0; n < 5; n++) {
			
			//Re-initializing data structures for this run
			start = 0;
			count = 0;
			max = 0;
			time = 0.0;
			finished = 0;
			proc_it = 0;
			proc_fl = 0;
			refs = create_linked_list();
			printf("\n\n\nGeneral execution\n");
		
			//Re-initialize memory map (only locations 0-99, where 100 is the terminating char)
			for (int o = 0; o < 100; o++) 
				mem_map[o] = '.';
			
			//Main loop; time measured in deciseconds/100 milliseconds
			while (time < 600.0) {
				
				//Check job list for new jobs, shifting iterator to next pending job
				while ( (min_jobs[m][proc_it].arrival <= time) && (proc_it < 150)) {
					//printf("\nchecking");
					
					//printf("a\n");
					if (count < 97 || proc_fl == 1) {		//Queue job based on page count or leaving process
						
						offset = time;
						
						for(int p = 0; p < min_jobs[m][proc_it].duration - 1; p++) {
							pr = (REF *)malloc(sizeof(REF));
							pr->job_id = proc_it;
							pr->index = min_jobs[m][proc_it].accesses[p];
							pr->time = offset;
							pr->last = 0;
							offset += 1.0;
							
							add_node(refs, pr);
						}
						
						pr = (REF *)malloc(sizeof(REF));
						pr->job_id = proc_it;
						pr->index = min_jobs[m][proc_it].accesses[min_jobs[m][proc_it].duration - 1];
						pr->time = offset;
						pr->last = 1;
						
						add_node(refs, pr);
						
						sort(refs, ref_cmp);
						
						printf("\n%2.2f \t%c \tENT \t%d \t %2.2f \n%s", time / 10, min_jobs[m][pr->job_id].char_id, min_jobs[m][pr->job_id].size, (float)(min_jobs[m][pr->job_id].duration) / 10, mem_map);
						
						proc_fl = 0;
						proc_it++;
					}
					else
						break;
				}
				
				//printf("b\n");
				
				//Check queue for any accesses active this timestamp
				while (refs->head != NULL) {
					//printf("\n%d\n", refs->head);
					
					if (((REF *)(refs->head->value))->time <= time && ((REF *)(refs->head->value))->time <= 600.0) {
						finished++;
					
						pr = refs->head->value;
						pr_value = *(REF *)(refs->head->value);
						op_out = (*rep[m])(pr->job_id, pr->index, min_jobs[m][pr->job_id].char_id);
						
						if (op_out.job_id == -1) {
							page_hit[m]++;
							//printf("\n%2.2f \t%c \t%d \tHIT", pr->time / 10, min_jobs[m][pr->job_id].char_id, pr->index); 
						} else if (op_out.job_id == -5) {
							//printf("\n%2.2f \t%c \t%d \tMISS", pr->time / 10, min_jobs[m][pr->job_id].char_id, pr->index); 
							page_miss[m]++;
							(void)0;

						} else {
							page_miss[m]++;
							//printf("\n%2.2f \t%c \t%d \tMISS\t%c %d", pr->time / 10, min_jobs[m][pr->job_id].char_id, pr->index, min_jobs[m][op_out.job_id].char_id, op_out.page_num); 
							(void)0;
						}
						
						remove_head(refs);
						
						if (pr_value.last == 1) {
							printf("\n%2.2f \t%c \tEXT \t%d \t %2.2f \n%s", time / 10, min_jobs[m][pr_value.job_id].char_id, min_jobs[m][pr_value.job_id].size, (float)(min_jobs[m][pr_value.job_id].duration) / 10, mem_map);
							proc_count[m]++;
							clear_proc(pr_value.job_id, min_jobs[m][pr_value.job_id].char_id);
							proc_fl = 1;
						}
					}
					else
						break;
					
					
				}
				
				
				//Increment time to next operation (between arriving processes and pending references)
				if ((proc_it < 150 && time < min_jobs[m][proc_it].arrival)) {
					time = min_jobs[m][proc_it].arrival;
				}
				if (refs->head != NULL) {
					time = ((REF *)(refs->head->value))->time;
					//printf("\n%2.2f %2.2f %2.2f %d", time, min_jobs[m][proc_it].arrival, ((REF *)(refs->head->value))->time, proc_it);
				}
				
				//else
					//printf("\n%2.2f %2.2f %d %d", time, min_jobs[m][proc_it].arrival, finished, proc_it);
				
				//Terminate if done iterating
				if ((proc_it > 149)) {
					time = 610.0;
				}
				
				
			}
			
			//Clear data
			while (refs->head != NULL) {
				remove_head(refs);
			}
			
		}
		
		//Run reference-based sim
		//Re-initializing data structures for this run
		start = 0;
		count = 0;
		max = 0;
		time = 0.0;
		finished = 0;
		proc_it = 0;
		proc_fl = 0;
		refs = create_linked_list();
		printf("\n\n\n100-reference run\n");
	
		//Re-initialize memory map (only locations 0-99, where 100 is the terminating char)
		for (int o = 0; o < 100; o++) 
			mem_map[o] = '.';
		
		//Main loop; time measured in deciseconds/100 milliseconds
		while (finished < 100) {
			
			//Check job list for new jobs, shifting iterator to next pending job
			while ( (min_jobs[m][proc_it].arrival <= time) && (proc_it < 150)) {
				//printf("\nchecking");
				
				//printf("a\n");
				if (count < 97 || proc_fl == 1) {		//Queue job based on page count or leaving process
					
					offset = time;
					
					for(int p = 0; p < min_jobs[m][proc_it].duration - 1; p++) {
						pr = (REF *)malloc(sizeof(REF));
						pr->job_id = proc_it;
						pr->index = min_jobs[m][proc_it].accesses[p];
						pr->time = offset;
						pr->last = 0;
						offset += 1.0;
						
						add_node(refs, pr);
					}
					
					pr = (REF *)malloc(sizeof(REF));
					pr->job_id = proc_it;
					pr->index = min_jobs[m][proc_it].accesses[min_jobs[m][proc_it].duration - 1];
					pr->time = offset;
					pr->last = 1;
					
					add_node(refs, pr);
					
					sort(refs, ref_cmp);
					
					printf("\n%2.2f \t%c \tENT \t%d \t %2.2f \n%s", time / 10, min_jobs[m][pr->job_id].char_id, min_jobs[m][pr->job_id].size, (float)(min_jobs[m][pr->job_id].duration) / 10, mem_map);
					
					proc_fl = 0;
					proc_it++;
				}
				else
					break;
			}
			
			//printf("b\n");
			
			//Check queue for any accesses active this timestamp
			while (refs->head != NULL) {
				//printf("\n%d\n", refs->head);
				
				if (((REF *)(refs->head->value))->time <= time && finished < 100) {
					finished++;
				
					pr = refs->head->value;
					pr_value = *(REF *)(refs->head->value);
					op_out = (*rep[m])(pr->job_id, pr->index, min_jobs[m][pr->job_id].char_id);
					
					if (op_out.job_id == -1) {
						page_hit[m]++;
						printf("\n%2.2f \t%c \t%d \tHIT", pr->time / 10, min_jobs[m][pr->job_id].char_id, pr->index); 
					} else if (op_out.job_id == -5) {
						printf("\n%2.2f \t%c \t%d \tMISS", pr->time / 10, min_jobs[m][pr->job_id].char_id, pr->index); 
						page_miss[m]++;
						(void)0;

					} else {
						page_miss[m]++;
						printf("\n%2.2f \t%c \t%d \tMISS\t%c %d", pr->time / 10, min_jobs[m][pr->job_id].char_id, pr->index, min_jobs[m][op_out.job_id].char_id, op_out.page_num); 
						(void)0;
					}
					
					remove_head(refs);
					
					if (pr_value.last == 1) {
						printf("\n%2.2f \t%c \tEXT \t%d \t %2.2f \n%s", time / 10, min_jobs[m][pr_value.job_id].char_id, min_jobs[m][pr_value.job_id].size, (float)(min_jobs[m][pr_value.job_id].duration) / 10, mem_map);
						//proc_count[m]++;
						clear_proc(pr_value.job_id, min_jobs[m][pr_value.job_id].char_id);
						proc_fl = 1;
					}
				}
				else
					break;
				
				
			}
			
			
			//Increment time to next operation (between arriving processes and pending references)
			if ((proc_it < 150 && time < min_jobs[m][proc_it].arrival)) {
				time = min_jobs[m][proc_it].arrival;
			}
			if (refs->head != NULL) {
				time = ((REF *)(refs->head->value))->time;
				//printf("\n%2.2f %2.2f %2.2f %d", time, min_jobs[m][proc_it].arrival, ((REF *)(refs->head->value))->time, proc_it);
			}
			
			//else
				//printf("\n%2.2f %2.2f %d %d", time, min_jobs[m][proc_it].arrival, finished, proc_it);
			
			//Terminate if done iterating
			if ((proc_it > 149)) {
				time = 610.0;
			}
			
			
		}
		
		//Clear data
		while (refs->head != NULL) {
			remove_head(refs);
		}
			
			
	
	}
	
	//Print output
	for (int q = 0; q < 5; q++) {
		page_hit[q] /= 5;
		page_miss[q] /= 5;
		proc_count[q] /= 5;
	}
	
	printf("\nAverage Performance \nAlg \tHits \tMisses \tRatio \tProcesses in 100 references");
	printf("\nFIFO \t%d \t%d \t%1.3f \t%3.2f", page_hit[0], page_miss[0], ((float)page_hit[0])/((float)page_miss[0]), proc_count[0]);
	printf("\nLRU \t%d \t%d \t%1.3f \t%3.2f", page_hit[1], page_miss[1], ((float)page_hit[1])/((float)page_miss[1]), proc_count[1]);
	printf("\nLFU \t%d \t%d \t%1.3f \t%3.2f", page_hit[2], page_miss[2], ((float)page_hit[2])/((float)page_miss[2]), proc_count[2]);
	printf("\nMFU \t%d \t%d \t%1.3f \t%3.2f", page_hit[3], page_miss[3], ((float)page_hit[3])/((float)page_miss[3]), proc_count[3]);
	printf("\nRAND \t%d \t%d \t%1.3f \t%3.2f\n", page_hit[4], page_miss[4], ((float)page_hit[4])/((float)page_miss[4]), proc_count[4]);
	
	//Free workload page reference arrays
	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 150; y++) {
			free(min_jobs[x][y].accesses);
		}
	}
}



int flt_cmp(const void *a, const void *b) {
	return ( *(float *)a > *(float *)b );
}



int generate_ref(int index, int pagecount) {
	if ((rand() % 10) > 2) 
		return (index + (rand() % 3) - 1 + pagecount) % pagecount;
	
	return (rand() % (pagecount - 3) + index + 2 + pagecount) % pagecount;
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



int page_check(int id, int pagenum, PAGE pg) {
	if (pg.job_id == id && pg.page_num == pagenum) {
		return 1;
	} else
		return -1;
}



int page_search(int id, int pagenum) {
	for (int i = start; i < start + count; i++) {
		if (page_check(id, pagenum, replacement_arr[i % 100]) == 1) {
			return i;
		}
	}
	
	return -1;
}



int pgc_cmp(const void *a, const void *b) {		//Takes pointers to PAGEs
	return ( ((PAGE *)a)->counter - ((PAGE *)b)->counter );
}



int ref_cmp(void *a, void *b) {		//Takes pointers to REFs
	return ( ((REF *)a)->time > ((REF *)b)->time );
}



void clear_proc(int id, char char_id) {
	int remaining;
	
	for (int i = 0; i < count; i++) {
		//printf("\nprecheck %d %d\n", i, count);
		if (replacement_arr[(i + start + 100) % 100].job_id == id) {
			//printf("\n%d\n", replacement_arr[(i + 100) % 100].mem_loc);
			
			remaining = count - i;
			//printf("\npreshift\n");
			for (int j = i; j < remaining; j++) { //Shift array
				//printf("\n%d %d %d\n", replacement_arr[(j + 100) % 100].job_id, replacement_arr[(j + 101) % 100].job_id, id);
				init_page(&replacement_arr[(j + start + 100) % 100], replacement_arr[(j + start + 101) % 100].job_id, replacement_arr[(j + start + 101) % 100].page_num, replacement_arr[(j + start + 101) % 100].counter, replacement_arr[(j + start + 101) % 100].mem_loc % 100);
			}
			//printf("\npostshift\n");
			
			count--;
			i--;
		}
	}
	
	for (int k = 0; k < 100; k++) {
		if (mem_map[k] == char_id)
			mem_map[k] = '.';
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
		for (int i = 0; i < 100; i++) {			//	Look for open location in memory map
			if (mem_map[i] == '.') {
				mem_loc = i;
				mem_map[i] = sym;
				break;
			}
		}
		init_page(&(replacement_arr[(start + count - 1) % 100]), id, pagenum, 0, mem_loc % 100);
		count++;
	} else {									//Case: need to replace
		index = (start + 100) % 100;								//	Swap first (first index) with new page
		mem_loc = replacement_arr[index].mem_loc;
		init_page(&output, replacement_arr[index].job_id, replacement_arr[index].page_num, 0, 0);
		init_page(&replacement_arr[index], id, pagenum, 0, mem_loc % 100);
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
		for (int i = 0; i < 100; i++) {			//	Look for open location in memory map
			if (mem_map[i] == '.') {
				mem_loc = i;
				mem_map[i] = sym;
				break;
			}
		}
		init_page(&(replacement_arr[count - 1]), id, pagenum, 0, mem_loc % 100);
		count++;
		
	} else {									//Case: need to replace
		index = rand() % count;
		mem_loc = replacement_arr[index].mem_loc;
		init_page(&output, replacement_arr[index].job_id, replacement_arr[index].page_num, 0, 0);
		init_page(&replacement_arr[index], id, pagenum, 0, mem_loc % 100);
		mem_map[mem_loc] = sym;					//	Rewrite location in memory map
	}
	
	return output;
}



PAGE lru_rep(int id, int pagenum, char sym) {	//Sorting array by counter value on each access
												//Start of array is always at index 0, end is count - 1
	PAGE output;								//Indexed in non-decreasing order
	int index = page_search(id, pagenum);
	int mem_loc;
	
	if (index > -1) {							//Case: page in array already
		replacement_arr[index].counter = max++;		//	Increment page's counter
		output.job_id = -1;
	} else if (count < 100) {					//Case: room to insert
		output.job_id = -5;
		for (int i = 0; i < 100; i++) {			//	Look for open location in memory map
			if (mem_map[i] == '.') {
				mem_loc = i;
				mem_map[i] = sym;
				break;
			}
		}
		init_page(&(replacement_arr[count - 1]), id, pagenum, max++, mem_loc % 100);
		count++;
	} else {									//Case: need to replace
		index = 0;								//	Swap LRU (first index) with new page
		mem_loc = replacement_arr[index].mem_loc;
		init_page(&output, replacement_arr[index].job_id, replacement_arr[index].page_num, 0, 0);
		init_page(&replacement_arr[index], id, pagenum, max++, mem_loc % 100);
		mem_map[mem_loc] = sym;					//	Rewrite location in memory map
	}
	
	qsort(replacement_arr, count, sizeof(PAGE), pgc_cmp);		//Sort array
	
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
		for (int i = 0; i < 100; i++) {			//	Look for open location in memory map
			if (mem_map[i] == '.') {
				mem_loc = i;
				mem_map[i] = sym;
				break;
			}
		}
		init_page(&(replacement_arr[count - 1]), id, pagenum, 0, mem_loc % 100);
		count++;
	} else {									//Case: need to replace
		index = 0;								//	Swap LFU (first index) with new page
		mem_loc = replacement_arr[index].mem_loc;
		init_page(&output, replacement_arr[index].job_id, replacement_arr[index].page_num, 0, 0);
		init_page(&replacement_arr[index], id, pagenum, 1, mem_loc % 100);
		mem_map[mem_loc] = sym;					//	Rewrite location in memory map
	}
	
	qsort(replacement_arr, count, sizeof(PAGE), pgc_cmp);		//Sort array
	
	return output;
}



PAGE mfu_rep(int id, int pagenum, char sym) {	//Sorting array by counter value on each access
												//Start of array is always at index 0, end is count - 1
	PAGE output;								//Indexed in non-decreasing order
	int index = page_search(id, pagenum);
	int mem_loc;
	
	if (index > -1) {							//Case: page in array already
		replacement_arr[index].counter++;		//	Increment page's counter
		output.job_id = -1;
	} else if (count < 100) {					//Case: room to insert
		output.job_id = -5;
		for (int i = 0; i < 100; i++) {			//	Look for open location in memory map
			if (mem_map[i] == '.') {
				mem_loc = i;
				mem_map[i] = sym;
				break;
			}
		}
		init_page(&(replacement_arr[count - 1]), id, pagenum, 0, mem_loc % 100);
		count++;
	} else {									//Case: need to replace
		index = count - 1;								//	Swap MFU (last index) with new page
		mem_loc = replacement_arr[index].mem_loc;
		init_page(&output, replacement_arr[index].job_id, replacement_arr[index].page_num, 0, 0);
		init_page(&replacement_arr[index], id, pagenum, 1, mem_loc % 100);
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


































