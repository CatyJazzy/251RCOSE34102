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

// 큐 노드 구조체
typedef struct QueueNode {
    Process* process;
    struct QueueNode* next;
} QueueNode;

// 큐 구조체
typedef struct {
    QueueNode* front;
    QueueNode* rear;
    int size;
} Queue;

// 스케줄러 구조체
typedef struct {
    Queue ready_queue;
    Queue waiting_queue;
    Process* running_process;
    int time_quantum;
    SchedulingAlgorithm current_algorithm;
} Scheduler;

// 스케줄러 관련 함수 선언
void init_scheduler(void);
void config_scheduler(SchedulingAlgorithm algorithm, int time_quantum);
void add_process(Process* process);
SchedulingResult* schedule(SchedulingAlgorithm algorithm, int time_quantum);
void print_scheduling_result(SchedulingResult* result);
void cleanup_scheduler(void);

// 큐 관련 함수 선언
void init_queue(Queue* queue);
void enqueue(Queue* queue, Process* process);
Process* dequeue(Queue* queue);
bool is_queue_empty(Queue* queue);
void clear_queue(Queue* queue);

#endif // SCHEDULER_H 