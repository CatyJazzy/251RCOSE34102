#include "process.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

Process* create_process(int pid, int arrival_time, int cpu_burst_time, int priority) {
    Process* process = (Process*)malloc(sizeof(Process));

    process->pid = pid;
    process->arrival_time = arrival_time;
    process->cpu_burst_time = cpu_burst_time;
    process->remaining_time = cpu_burst_time; // 초기값은 전체 cpu burst time일 것임
    process->priority = priority;

    process->state = NEW;

    process->waiting_time = 0;
    process->turnaround_time = 0;
    process->response_time = 0;
    process->completion_time = 0;
    process->is_first_execution = true;

    // process->io_count = rand() % (MAX_IO_COUNT - 1) + 2; // 2~3개 임의 설정
    
    // NOTE - i/o작업 설정 내용은 Config함수에서 Cpu burst time을 입력받은 후에 진행
    process->io_count = 0; // 임시 설정

    process->current_io_idx = 0;
    process->io_remaining_time = 0;
    process->is_doing_io = false;
    process->total_io_time_spent = 0;

    return process;
}

void destroy_process(Process* process) {
    free(process);
}

void print_process(Process* process) {
    printf("pid: %d, 도착 시간: %d, CPU 실행시간: %d, 우선순위: %d\n", 
    process->pid, process->arrival_time, process->cpu_burst_time, process->priority);
    printf("IO 요청 시간: ");
    for (int i=0; i<process->io_count; i++) {
        printf("%d ", process->io_request_times[i]);
    }
    printf("\n");
    printf("IO 실행 시간: ");
    for (int i=0; i<process->io_count; i++) {
        printf("%d ", process->io_burst_times[i]);
    }
    printf("\n\n");
}

// NOTE - https://en.wikipedia.org/wiki/Bubble_sort (버블정렬 활용)
void sort_io_requests(Process* process) {
    for (int i=0; i<process->io_count-1; i++) {
        for (int j=0; j<process->io_count-i-1; j++) {
            if (process->io_request_times[j] > process->io_request_times[j + 1]) {
                int temp_timing = process->io_request_times[j];
                process->io_request_times[j] = process->io_request_times[j + 1];
                process->io_request_times[j + 1] = temp_timing;

                int temp_burst = process->io_burst_times[j];
                process->io_burst_times[j] = process->io_burst_times[j + 1];
                process->io_burst_times[j + 1] = temp_burst;
            }
        }
    }
}