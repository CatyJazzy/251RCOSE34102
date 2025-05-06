#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

// 스케줄링 알고리즘 타입
typedef enum {
    FCFS,
    SJF_NON_PREEMPTIVE,
    SJF_PREEMPTIVE,
    PRIORITY_NON_PREEMPTIVE,
    PRIORITY_PREEMPTIVE,
    ROUND_ROBIN
} SchedulingAlgorithm;

// 스케줄링 결과를 저장하는 구조체
typedef struct {
    double avg_waiting_time;
    double avg_turnaround_time;
    int* gantt_chart;
    int gantt_chart_size;
} SchedulingResult;

// 스케줄러 관련 함수 선언
void init_scheduler(void);
void add_process(Process* process);
SchedulingResult* schedule(SchedulingAlgorithm algorithm, int time_quantum);
void print_scheduling_result(SchedulingResult* result);
void cleanup_scheduler(void);

#endif // SCHEDULER_H 