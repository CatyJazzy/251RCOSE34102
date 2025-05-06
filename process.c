#include <stdio.h>
#include <stdlib.h>
#include "process.h"

Process* create_process(int pid, int arrival_time, int cpu_burst_time, 
                       int io_burst_time, int io_request_time, int priority) {
    Process* process = (Process*)malloc(sizeof(Process));
    if (process == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }

    process->pid = pid;
    process->arrival_time = arrival_time;
    process->cpu_burst_time = cpu_burst_time;
    process->remaining_time = cpu_burst_time;
    process->io_burst_time = io_burst_time;
    process->io_request_time = io_request_time;
    process->priority = priority;
    process->waiting_time = 0;
    process->turnaround_time = 0;
    process->state = NEW;
    process->is_io_completed = false;

    return process;
}

void destroy_process(Process* process) {
    if (process != NULL) {
        free(process);
    }
}

void print_process_info(Process* process) {
    if (process == NULL) {
        printf("프로세스가 NULL입니다.\n");
        return;
    }

    printf("프로세스 ID: %d\n", process->pid);
    printf("도착 시간: %d\n", process->arrival_time);
    printf("CPU 버스트 시간: %d\n", process->cpu_burst_time);
    printf("남은 실행 시간: %d\n", process->remaining_time);
    printf("I/O 버스트 시간: %d\n", process->io_burst_time);
    printf("I/O 요청 시간: %d\n", process->io_request_time);
    printf("우선순위: %d\n", process->priority);
    printf("대기 시간: %d\n", process->waiting_time);
    printf("반환 시간: %d\n", process->turnaround_time);
    printf("상태: %d\n", process->state);
    printf("I/O 완료 여부: %s\n", process->is_io_completed ? "예" : "아니오");
    printf("------------------------\n");
} 