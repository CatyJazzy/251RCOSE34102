#include "scheduler.h"
#include <stdio.h>

void check_and_add_arrived_processes(Scheduler* scheduler, int current_time) {
    for (int i=0; i<scheduler->process_cnt; i++) {
        if (scheduler->process_arr[i]->arrival_time == current_time && scheduler->process_arr[i]->state == NEW) {
            scheduler->ready_queue[scheduler->ready_queue_cnt++] = scheduler->process_arr[i];
            scheduler->process_arr[i]->state = READY;
        }
    }
}

void process_io_operations(Scheduler* scheduler, int* terminated_process_cnt) {
    for (int i=0; i<scheduler->waiting_queue_cnt; i++) {
            Process* process = scheduler->waiting_queue[i]; // io작업 처리할 프로세스
            process->io_remaining_time--;
            
            // 완료되었을 때
            if (process->io_remaining_time <= 0) {
                process->is_doing_io = false;
                process->relative_cpu_execution_time = 0;
                
                
                if(process->remaining_time > 0) {
                    scheduler->ready_queue[scheduler->ready_queue_cnt++] = process;
                    process->state = READY;
                } else if (process->current_io_idx >= process->io_count) {
                    process->state = TERMINATED;
                    (*terminated_process_cnt)++;
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
}

void handle_io_task_of_process(Process* process, Scheduler* scheduler, int* is_moved_to_waiting) {
     if (process->current_io_idx < process->io_count && 
                    process->io_request_times[process->current_io_idx] == process->relative_cpu_execution_time) {
                process->is_doing_io = true;
                process->io_remaining_time = process->io_burst_times[process->current_io_idx];
                process->state = WAITING;
                scheduler->waiting_queue[scheduler->waiting_queue_cnt++] = process;
                process->current_io_idx++;  // 다음 io로 인덱스 변경
                *is_moved_to_waiting = true;
            }
}

void remove_from_ready_queue(Scheduler* scheduler, int idx) {
    for (int i=idx; i<scheduler->ready_queue_cnt-1; i++) {
        scheduler->ready_queue[i] = scheduler->ready_queue[i+1];
    }
    scheduler->ready_queue_cnt--;
}

void handle_gantt_chart_idle(Scheduler* scheduler, int* is_idle, GanttChart* idle_item, int current_simulation_time) {
    if (!(*is_idle)) {
        idle_item-> start_time = current_simulation_time;
        sprintf(idle_item->process_name, "IDLE");
        *is_idle = 1;
    }
}


void end_gantt_chart_idle(Scheduler* scheduler, int* is_idle, GanttChart* idle_item, int current_simulation_time) {
    if (*is_idle) {
        idle_item->end_time = current_simulation_time;
        scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = *idle_item;
        *is_idle = 0;
    }
}

void print_scheduling_debug_info(Scheduler* scheduler, Process* current_process, int current_time) {
    printf("\n=== 디버깅 정보 (시간: %d) ===\n", current_time);
    if (current_process) {
        printf("현재 실행 중인 프로세스: P%d\n", current_process->pid);
        printf("- 남은 실행 시간: %d\n", current_process->remaining_time);
        printf("- 우선순위: %d\n", current_process->priority);
        printf("- 상태: %s\n", current_process->state == RUNNING ? "RUNNING" : "READY");
    } else {
        printf("현재 실행 중인 프로세스: 없음\n");
    }

    printf("\nReady Queue 상태:\n");
    for (int i = 0; i < scheduler->ready_queue_cnt; i++) {
        printf("P%d (우선순위: %d, 남은시간: %d)\n", 
            scheduler->ready_queue[i]->pid,
            scheduler->ready_queue[i]->priority,
            scheduler->ready_queue[i]->remaining_time);
    }

    printf("\nWaiting Queue 상태:\n");
    for (int i = 0; i < scheduler->waiting_queue_cnt; i++) {
        printf("P%d (IO 남은시간: %d)\n", 
            scheduler->waiting_queue[i]->pid,
            scheduler->waiting_queue[i]->io_remaining_time);
    }
    printf("===========================\n\n");
}