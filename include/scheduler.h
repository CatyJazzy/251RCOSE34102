#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

typedef struct {
    int start_time;
    int end_time;
    char process_name[10]; // "P100000" 최대 아이디 길이 고려 
} GanttChart;

// 스케줄링 알고리즘 타입 정의 
typedef enum {
    FCFS,
    SJF_NP, // 비선점
    SJF_P, // 선점
    PRIORITY_NP,
    PRIORITY_P,
    ROUND_ROBIN
} Algorithm;


typedef struct {
    Process** process_arr;
    int process_cnt; // 시뮬레이션 ㅇ할 프로세스 개수 (입력제한용)


    Process** ready_queue;
    int ready_queue_cnt;
    Process** waiting_queue;
    int waiting_queue_cnt;

    Algorithm scheduling_algorithm;
    
    int time_quantum;
    // int current;

    GanttChart gantt_chart[100000];
    int gantt_chart_cnt;
} Scheduler;

Scheduler* Config(void);
Scheduler* Simulate(Scheduler* scheduler);

void Evaluation(Scheduler* scheduler);

void schedule_fcfs(Scheduler* scheduler); 
void schedule_sjf_np(Scheduler* scheduler);
void schedule_sjf_p(Scheduler* scheduler);
void schedule_priority_np(Scheduler* scheduler);
void schedule_priority_p(Scheduler* scheduler);
void schedule_round_robin(Scheduler* scheduler);
#endif