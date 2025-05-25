#include "process.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

Process* create_process(int arrival_time, int cpu_burst_time, int priority) {
    Process* process = (Process*)malloc(sizeof(Process));

    // pid로 랜덤으로 선택된 숫자 사용 (0~10000) 
    // TODO - 같은 랜덤값 생성되었을 때? 중복 검증로직
    srand(time(NULL));
    int random_number = rand() % 10001;

    process->pid = random_number;
    process->arrival_time = arrival_time;
    process->cpu_burst_time = cpu_burst_time;
    process->priority = priority;

    process->state = NEW;

    process->waiting_time = 0;
    process->turnaround_time = 0;
    process->response_time = 0;

    process->io_count = rand() % (MAX_IO_COUNT - 1) + 2;
    for (int i=0; i<process->io_count; i++) {
        // NOTE - 임시 초기화 후 이후 입력받을 때(config에서) 재정의 
        process->io_request_times[i] = rand() % 5; 
        process->io_burst_times[i] = rand() % 3 + 1; // 1~3시간 임의 설정
    }

    process->current_io_idx = 0;
    process->io_remaining_time = 0;
    process->is_doing_io = FALSE;

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