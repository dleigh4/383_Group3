#include<stdio.h>
#include<stdbool.h>
#include<limits.h>
#include<stdlib.h>
#include<time.h>

struct process
{
  int process_id;
  int arrival_time;
  int brust_time;
  int completion_time,wt,tat,rt,start_time;
}proc[100];

int findmax(int a, int b)
{
    return a>b?a:b;
}

int findmin(int a, int b)
{
    return a<b?a:b;
}

int main()
{
    srand(time(NULL));
    int n;
    bool is_completed[100]={false},is_first_process=true;
    int current_time = 0;
    int completed = 0;;
    printf("Enter total number of processes: ");
    scanf("%d",&n);
    int sum_tat=0,sum_wt=0,sum_rt=0,total_idle_time=0,prev=0,length_cycle;
    float cpu_utilization;
    int max_completion_time,min_arrival_time;

    for(int i=0;i<n;i++)
    {
        proc[i].arrival_time = rand() % 100;
        proc[i].process_id = i+1;
    }
    
    for(int i=0;i<n;i++)
    {
        proc[i].brust_time = (rand() % 10) + 1;
    }
    
    while(completed!=n)
    {
        //find process with min. burst time in ready queue at current time
        int min_index = -1;
        int minimum = INT_MAX;
        for(int i = 0; i < n; i++) {
            if(proc[i].arrival_time <= current_time && is_completed[i] == false) {
                if(proc[i].brust_time < minimum) {
                    minimum = proc[i].brust_time;
                    min_index = i;
                }
                if(proc[i].brust_time == minimum) {
                    if(proc[i].arrival_time < proc[min_index].arrival_time) {
                        minimum= proc[i].brust_time;
                        min_index = i;
                    }
                }
            }
        }

        if(min_index==-1)
        {
            current_time++;
        }
        else
        {
        proc[min_index].start_time = current_time;
        proc[min_index].completion_time = proc[min_index].start_time + proc[min_index].brust_time;
        proc[min_index].tat = proc[min_index].completion_time - proc[min_index].arrival_time;
        proc[min_index].wt = proc[min_index].tat - proc[min_index].brust_time;
        proc[min_index].rt = proc[min_index].wt;
        // proc[min_index].rt = proc[min_index].start_time - proc[min_index].at;
                
        sum_tat +=proc[min_index].tat;
        sum_wt += proc[min_index].wt;
        sum_rt += proc[min_index].rt;
        total_idle_time += (is_first_process==true) ? 0 : (proc[min_index].start_time -  prev);
        
        completed++;
        is_completed[min_index]=true;
        current_time = proc[min_index].completion_time;
        prev= current_time;
        is_first_process = false;
        }
    }
    
    //Calculate Length of Process completion cycle
    max_completion_time = INT_MIN;
    min_arrival_time = INT_MAX;
    for(int i=0;i<n;i++)
    {
        max_completion_time = findmax(max_completion_time,proc[i].completion_time);
        min_arrival_time = findmin(min_arrival_time,proc[i].arrival_time);
    }
    length_cycle = max_completion_time - min_arrival_time;

    //Output
    printf("\nProcess ID\tAT\tBurst Time\tCT\tTAT\tWT\tRT\n");
    for(int i=0;i<n;i++)
     printf("%d\t\t%d\t%d\t\t%d\t%d\t%d\t%d\n",proc[i].process_id,proc[i].arrival_time,proc[i].brust_time,proc[i].completion_time,proc[i].tat,proc[i].wt,proc[i].rt);

    printf("\n");
    
    cpu_utilization = (float)(length_cycle - total_idle_time)/ length_cycle;

    printf("\nAverage Turn Around time= %.2f ",(float)sum_tat/n);
    printf("\nAverage Waiting Time= %.2f ",(float)sum_wt/n);
    printf("\nAverage Response Time= %.2f ",(float)sum_rt/n);
    printf("\nThroughput = %.2f",n/(float)length_cycle);
    printf("\nCPU Utilization = %.2f\n",cpu_utilization*100);
    return 0;
}
