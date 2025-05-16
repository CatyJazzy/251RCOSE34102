#include <stdlib.h>
#include <stdio.h>
#include "scheduler.h"

// 전역 스케줄러 인스턴스
Scheduler scheduler;

// 큐 초기화
void init_queue(Queue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

// 큐에 프로세스 추가
void enqueue(Queue* queue, Process* process) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (new_node == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    
    new_node->process = process;
    new_node->next = NULL;
    
    if (is_queue_empty(queue)) {
        queue->front = new_node;
    } else {
        queue->rear->next = new_node;
    }
    queue->rear = new_node;
    queue->size++;
}

// 큐에서 프로세스 제거
Process* dequeue(Queue* queue) {
    if (is_queue_empty(queue)) {
        return NULL;
    }
    
    QueueNode* temp = queue->front;
    Process* process = temp->process;
    
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    
    free(temp);
    queue->size--;
    return process;
}

// 큐가 비어있는지 확인
bool is_queue_empty(Queue* queue) {
    return queue->front == NULL;
}

// 큐 비우기
void clear_queue(Queue* queue) {
    while (!is_queue_empty(queue)) {
        dequeue(queue);
    }
}

// 스케줄러 초기화
void init_scheduler(void) {
    init_queue(&scheduler.ready_queue);
    init_queue(&scheduler.waiting_queue);
    scheduler.running_process = NULL;
    scheduler.time_quantum = 0;
    scheduler.current_algorithm = FCFS;
}

// 스케줄러 설정
void config_scheduler(SchedulingAlgorithm algorithm, int time_quantum) {
    scheduler.current_algorithm = algorithm;
    scheduler.time_quantum = time_quantum;
    
    // 큐 초기화
    clear_queue(&scheduler.ready_queue);
    clear_queue(&scheduler.waiting_queue);
    scheduler.running_process = NULL;
}

// 프로세스 추가
void add_process(Process* process) {
    if (process == NULL) return;
    
    // 프로세스 상태를 READY로 변경
    process->state = READY;
    enqueue(&scheduler.ready_queue, process);
}

// 스케줄링 알고리즘 실행
SchedulingResult* schedule(SchedulingAlgorithm algorithm, int time_quantum) {
    // TODO: 각 스케줄링 알고리즘 구현
    return NULL;
}

// 스케줄링 결과 출력
void print_scheduling_result(SchedulingResult* result) {
    if (result == NULL) return;
    
    printf("\n스케줄링 결과:\n");
    printf("평균 대기 시간: %.2f\n", result->avg_waiting_time);
    printf("평균 반환 시간: %.2f\n", result->avg_turnaround_time);
    
    if (result->gantt_chart != NULL) {
        printf("\n간트 차트:\n");
        for (int i = 0; i < result->gantt_chart_size; i++) {
            printf("%d ", result->gantt_chart[i]);
        }
        printf("\n");
    }
}

// 스케줄러 정리
void cleanup_scheduler(void) {
    clear_queue(&scheduler.ready_queue);
    clear_queue(&scheduler.waiting_queue);
    scheduler.running_process = NULL;
} 