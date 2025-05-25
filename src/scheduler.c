#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

Scheduler* Config() {
    Scheduler* scheduler = (Scheduler*)malloc(sizeof(Scheduler));
    int process_cnt;
    int scheduling_algorithm;
    int time_quantum;

    printf("|---- 시스템 초기화를 위한 정보 입력 ----|\n");
    printf("1. 시뮬레이션 할 프로세스 개수를 입력하세요 : ");
    scanf("%d", &process_cnt);
    // TODO - 잘못된 값 입력 예외처리 (일단 FCFS를 기본으로)
    printf("2. 시뮬레이션 할 스케줄링 알고리즘을 선택하세요\n");
    printf("(1) FCFS\n(2) SJF-비선점\n(3) SJF-선점형\n(4) PRIORITY-비선점형\n(5) PRIORITY-선점형\n(6) ROUND_ROBIN\n");

    scanf("%d", &scheduling_algorithm);
    switch (scheduling_algorithm) {
        case 1:
            scheduler->scheduling_algorithm = FCFS;
            break;
        case 2:
            scheduler->scheduling_algorithm = SJF_NP;
            break;
        case 3:
            scheduler->scheduling_algorithm = SJF_P;
            break;
        case 4:
            scheduler->scheduling_algorithm = PRIORITY_NP;
            break;
        case 5:
            scheduler->scheduling_algorithm = PRIORITY_P;
            break;
        case 6:
            scheduler->scheduling_algorithm = ROUND_ROBIN;
            break;
        default:
            printf("잘못된 알고리즘 선택입니다! 기본값인 FCFS로 설정됩니다.\n");
            scheduler->scheduling_algorithm = FCFS;
    }

    printf("\n3. 시뮬레이션 할 타임 퀀텀을 입력하세요 : ");
    scanf("%d", &time_quantum);
    printf("|---- 시스템 초기화 완료 ----|\n\n\n");

    scheduler->process_cnt = process_cnt;
    scheduler->time_quantum = time_quantum;

    scheduler->process_arr = (Process**)malloc(sizeof(Process*) * process_cnt);
    // NOTE - 일단 전체 프로세스 개수로 지정
    scheduler->ready_queue = (Process**)malloc(sizeof(Process*) * process_cnt); 
    scheduler->waiting_queue = (Process**)malloc(sizeof(Process*) * process_cnt);

    scheduler->ready_queue_cnt = 0;
    scheduler->waiting_queue_cnt = 0;

    printf("|---- 시뮬레이션할 프로세스 정보 입력 ----|\n");
    // 프로세스 생성
    for (int i = 0; i < process_cnt; i++) {
         // TODO - 터무니 없는 값 예외처리 (또는 모순되는 값)
        scheduler->process_arr[i] = create_process(0, 0, 0);
        printf("%d번째 프로세스의 도착 시간을 입력하세요 : ", i+1);
        scanf("%d", &scheduler->process_arr[i]->arrival_time);
        printf("%d번째 프로세스의 CPU 실행 시간을 입력하세요 : ", i+1);
        scanf("%d", &scheduler->process_arr[i]->cpu_burst_time);
        scheduler->process_arr[i]->remaining_time = scheduler->process_arr[i]->cpu_burst_time;  
        printf("%d번째 프로세스의 우선순위를 입력하세요 : ", i+1);
        scanf("%d", &scheduler->process_arr[i]->priority);
        printf("\n");
    }

    for (int i=0; i<process_cnt; i++) {
        for (int j=0; j<scheduler->process_arr[i]->io_count; j++) {
           scheduler->process_arr[i]->io_request_times[j] = (rand() % (scheduler->process_arr[i]->cpu_burst_time - 1)) + 1;
        }
        sort_io_requests(scheduler->process_arr[i]);
    }

    printf("|---- 프로세스 정보 입력 완료 ----|\n\n\n");

    return scheduler;
}

Scheduler* Simulate(Scheduler* scheduler) {
    switch (scheduler->scheduling_algorithm) {
        case FCFS:
            schedule_fcfs(scheduler);
            break;
        case SJF_NP:
            schedule_sjf_np(scheduler);
            break;
        case SJF_P:
            break;
        case PRIORITY_NP:
            break;
        case PRIORITY_P:
            break;
        case ROUND_ROBIN:
            break;
        default:
            break;
    }   
    return scheduler;
}

void schedule_sjf_np(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;

    while(terminated_process_cnt < scheduler->process_cnt) {
        // 도착한 프로세스 확인함
        for (int i=0; i<scheduler->process_cnt; i++) {
            if (scheduler->process_arr[i]->arrival_time == current_simulation_time && scheduler->process_arr[i]->state == NEW) {
                scheduler->ready_queue[scheduler->ready_queue_cnt++] = scheduler->process_arr[i];
                scheduler->process_arr[i]->state = READY;
            }
        }

        // I/O작업을 처리함
        for (int i=0; i<scheduler->waiting_queue_cnt; i++) {
            Process* process = scheduler->waiting_queue[i]; // io작업 처리할 프로세스
            process->io_remaining_time--;
            
            // 완료되었을 때
            if (process->io_remaining_time <= 0) {
                process->is_doing_io = false;
                
                if(process->remaining_time > 0) {
                    scheduler->ready_queue[scheduler->ready_queue_cnt++] = process;
                    process->state = READY;
                } else if (process->current_io_idx >= process->io_count) {
                    process->state = TERMINATED;
                    terminated_process_cnt++;
                    printf("P%d가 종료되었습니다.\n", process->pid);
                }
                
                // waiting-Q 제거로직
                for (int j=i; j<scheduler->waiting_queue_cnt-1; j++) {
                    scheduler->waiting_queue[j] = scheduler->waiting_queue[j+1];
                }
                scheduler->waiting_queue_cnt--;
                i--; 
            }
        }
        
        if (scheduler->ready_queue_cnt > 0) {
            Process* current_process = scheduler->ready_queue[0]; // 임시 설정

            for (int i=0; i<scheduler->ready_queue_cnt; i++) {
                if (scheduler->ready_queue[i]->remaining_time < current_process->remaining_time) {
                    current_process = scheduler->ready_queue[i];
                }
            }

            current_process->state = RUNNING;
            current_process->remaining_time -= 1;   

             int is_moved_to_waiting = false;

            // io작업 처리 (요청시점 동일한 경우를 위한 루프)
            if (current_process->current_io_idx < current_process->io_count && 
                    current_process->io_request_times[current_process->current_io_idx] == current_process->cpu_burst_time - current_process->remaining_time) {

                current_process->is_doing_io = true;
                current_process->io_remaining_time = current_process->io_burst_times[current_process->current_io_idx];
                current_process->state = WAITING;
                scheduler->waiting_queue[scheduler->waiting_queue_cnt++] = current_process;
                current_process->current_io_idx++;  // 다음 io로 인덱스 변경
                is_moved_to_waiting = true;
            }

            // ready-Q 제거
            if (is_moved_to_waiting) {
                for (int j=0; j<scheduler->ready_queue_cnt-1; j++) {
                    scheduler->ready_queue[j] = scheduler->ready_queue[j+1];
                }
                scheduler->ready_queue_cnt--;
            } else if (current_process ->remaining_time <= 0 &&current_process->is_doing_io == false) {
                current_process->state = TERMINATED;
                terminated_process_cnt++;
                printf("P%d가 종료되었습니다.\n", current_process->pid);

                // ready-Q 제거로직
                for (int j=0; j<scheduler->ready_queue_cnt-1; j++) {
                    scheduler->ready_queue[j] = scheduler->ready_queue[j+1];
                }
                scheduler->ready_queue_cnt--;
            }   
        }
        printf("현재 시간: %d\n", current_simulation_time);
        current_simulation_time++;
    }

}


void schedule_fcfs(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;

    while (terminated_process_cnt < scheduler->process_cnt) {
        // 도착한 프로세스 확인함
        for (int i=0; i<scheduler->process_cnt; i++) {
            if (scheduler->process_arr[i]->arrival_time == current_simulation_time && scheduler->process_arr[i]->state == NEW) {
                scheduler->ready_queue[scheduler->ready_queue_cnt++] = scheduler->process_arr[i];
                scheduler->process_arr[i]->state = READY;
            }
        }

        for (int i=0; i<scheduler->waiting_queue_cnt; i++) {
            Process* process = scheduler->waiting_queue[i]; // io작업 처리할 프로세스
            process->io_remaining_time--;
            
            // 완료되었을 때
            if (process->io_remaining_time <= 0) {
                process->is_doing_io = false;
                
                if(process->remaining_time > 0) {
                    scheduler->ready_queue[scheduler->ready_queue_cnt++] = process;
                    process->state = READY;
                } else if (process->current_io_idx >= process->io_count) {
                    process->state = TERMINATED;
                    terminated_process_cnt++;
                    printf("P%d가 종료되었습니다.\n", process->pid);
                }
                
                // waiting-Q 제거로직
                for (int j=i; j<scheduler->waiting_queue_cnt-1; j++) {
                    scheduler->waiting_queue[j] = scheduler->waiting_queue[j+1];
                }
                scheduler->waiting_queue_cnt--;
                i--; 
            }
        }

        if (scheduler->ready_queue_cnt > 0) {
            Process* process = scheduler->ready_queue[0]; // FCFS이므로 맨 앞에 있는 프로세스 선택
            
            process->state = RUNNING;
            process->remaining_time -= 1; // CPU 실행 처리

            int is_moved_to_waiting = false;

            // io작업 처리 (요청시점 동일한 경우를 위한 루프)
            if (process->current_io_idx < process->io_count && 
                    process->io_request_times[process->current_io_idx] == process->cpu_burst_time - process->remaining_time) {

                process->is_doing_io = true;
                process->io_remaining_time = process->io_burst_times[process->current_io_idx];
                process->state = WAITING;
                scheduler->waiting_queue[scheduler->waiting_queue_cnt++] = process;
                process->current_io_idx++;  // 다음 io로 인덱스 변경
                is_moved_to_waiting = true;
            }

            // ready-Q 제거
            if (is_moved_to_waiting) {
                for (int j=0; j<scheduler->ready_queue_cnt-1; j++) {
                    scheduler->ready_queue[j] = scheduler->ready_queue[j+1];
                }
                scheduler->ready_queue_cnt--;
            } else if (process ->remaining_time <= 0 &&process->is_doing_io == false) {
                process->state = TERMINATED;
                terminated_process_cnt++;
                printf("P%d가 종료되었습니다.\n", process->pid);

                // ready-Q 제거로직
                for (int j=0; j<scheduler->ready_queue_cnt-1; j++) {
                    scheduler->ready_queue[j] = scheduler->ready_queue[j+1];
                }
                scheduler->ready_queue_cnt--;
            }
                
        }
        printf("현재 시간: %d\n", current_simulation_time);
        current_simulation_time++;
    }
}
