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
                    process->io_request_times[process->current_io_idx] == process->cpu_burst_time - process->remaining_time) {

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
