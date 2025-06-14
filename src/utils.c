#include "scheduler.h"
#include <stdio.h>

void update_arrivals(Scheduler* scheduler, int current_time) {
    // ANCHOR (데모 이후 수정 06.11) I/O 작업이 완료되어 ready queue에 추가될 예정인 프로세스들을 수집
    Process* completed_io_processes[scheduler->process_cnt];
    int completed_count = 0;
    
    for (int i=0; i<scheduler->process_cnt; i++) {
        if (scheduler->process_arr[i]->will_be_ready > 0) {
            completed_io_processes[completed_count++] = scheduler->process_arr[i];
        }
    }
    
    // will_be ready 순서 번호대로 정렬 (작을수록 우선)
    for (int i=0; i<completed_count-1; i++) {
        for (int j=i+1; j<completed_count; j++) {
            if (completed_io_processes[i]->will_be_ready > completed_io_processes[j]->will_be_ready) {
                Process* temp = completed_io_processes[i];
                completed_io_processes[i] = completed_io_processes[j];
                completed_io_processes[j] = temp;
            }
        }
    }
    
    // 정렬된 순서대로 ready queue에 추가함
    for (int i=0; i<completed_count; i++) {
        scheduler->ready_queue[scheduler->ready_queue_cnt++] = completed_io_processes[i];
        completed_io_processes[i]->will_be_ready = 0; // 초기화
    }

    // 새로운 프로세스 도착 처리 (I/O 완료 프로세스 다음에 처리)
    for (int i=0; i<scheduler->process_cnt; i++) {
        if (scheduler->process_arr[i]->arrival_time == current_time && scheduler->process_arr[i]->state == NEW) {
            scheduler->ready_queue[scheduler->ready_queue_cnt++] = scheduler->process_arr[i];
            scheduler->process_arr[i]->state = READY;
        }
    }
}

void process_io_operations(Scheduler* scheduler, int* terminated_process_cnt) {
    static int ready_order = 1; // I/O 완료 순서를 위한 static 변수
    
    for (int i=0; i<scheduler->waiting_queue_cnt; i++) {
            Process* process = scheduler->waiting_queue[i]; // io작업 처리할 프로세스
            process->io_remaining_time--;
            
            // 완료되었을 때
            if (process->io_remaining_time <= 0) {
                process->is_doing_io = false;
                
                if(process->remaining_time > 0) {
                    // ANCHOR (06.11 - 데모 이후 수정) 다음 시간 단위에 ready queue에 추가하기 위해 상태만 변경하도록 수정
                    process->state = READY;
                    process->will_be_ready = ready_order++;  // 순서 번호 할당 (1부터 시작, 0은 false를 의미함.)
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

void handle_io_task_of_process(Process* process, Scheduler* scheduler, int* is_io_request_exists) {
     if (process->current_io_idx < process->io_count && 
                    process->io_request_times[process->current_io_idx] == (process->cpu_burst_time - process->remaining_time)) {
                process->is_doing_io = true;
                process->io_remaining_time = process->io_burst_times[process->current_io_idx];
                process->state = WAITING;
                scheduler->waiting_queue[scheduler->waiting_queue_cnt++] = process;
                process->current_io_idx++;  // 다음 io로 인덱스 변경
                *is_io_request_exists = true;
            }
}

void remove_from_ready_queue(Scheduler* scheduler, int idx) {
    for (int i=idx; i<scheduler->ready_queue_cnt-1; i++) {
        scheduler->ready_queue[i] = scheduler->ready_queue[i+1];
    }
    scheduler->ready_queue_cnt--;
}

void update_idle_state(Scheduler* scheduler, int* is_idle, GanttChart* idle_item, int current_simulation_time, Process* current_process) {
    if (!(*is_idle) && scheduler->ready_queue_cnt == 0 && current_process == NULL) {
        idle_item-> start_time = current_simulation_time;
        sprintf(idle_item->process_name, "IDLE");
        *is_idle = 1;
    }

    if ((*is_idle)) {
        if (scheduler->ready_queue_cnt > 0 || current_process != NULL) {
            idle_item-> end_time = current_simulation_time;
            scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = *idle_item;
            *is_idle = 0;
        }
    }
}

bool is_new_process_need(Scheduler* scheduler, Process * current_process) {
    return (current_process == NULL && scheduler->ready_queue_cnt > 0);
}

Process* select_earlier_process(Scheduler* scheduler, Process* current_process) {
    if (scheduler->ready_queue_cnt <= 0) {
        return NULL;
    }

    Process* early_process = scheduler->ready_queue[0];
    int early_process_idx = 0;
    
    remove_from_ready_queue(scheduler, early_process_idx);
    return early_process;
}

Process* select_shortest_process(Scheduler* scheduler, Process* current_process) {
    if (scheduler->ready_queue_cnt <= 0) {
        return NULL;
    }

    Process* shortest_process = scheduler->ready_queue[0];
    int shortest_process_idx = 0;

    for (int i=1; i<scheduler->ready_queue_cnt; i++) {
        if (scheduler->ready_queue[i]->cpu_burst_time < shortest_process->cpu_burst_time) {
            shortest_process = scheduler->ready_queue[i];
            shortest_process_idx = i;
        } else if (scheduler->ready_queue[i]->cpu_burst_time == shortest_process->cpu_burst_time) {
            // 동점 시 도착시간 기준
            if (scheduler->ready_queue[i]->arrival_time < shortest_process->arrival_time) {
                shortest_process = scheduler->ready_queue[i];
                shortest_process_idx = i;
            } else if (scheduler->ready_queue[i]->arrival_time == shortest_process->arrival_time) {
                // 도착시간도 동일하면 프로세스 ID가 더 작은 것 선택
                if (scheduler->ready_queue[i]->pid < shortest_process->pid) {
                    shortest_process = scheduler->ready_queue[i];
                    shortest_process_idx = i;
                }
            }
        }
    }
    
    remove_from_ready_queue(scheduler, shortest_process_idx);
    return shortest_process;
}

Process* select_shortest_remaining_process(Scheduler* scheduler, Process* current_process) {
    if (scheduler->ready_queue_cnt <= 0) {
        return NULL;
    }

    Process* shortest_process = scheduler->ready_queue[0];
    int shortest_process_idx = 0;

    for (int i=1; i<scheduler->ready_queue_cnt; i++) {
        if (scheduler->ready_queue[i]->remaining_time < shortest_process->remaining_time) {
            shortest_process = scheduler->ready_queue[i];
            shortest_process_idx = i;
        } else if (scheduler->ready_queue[i]->remaining_time == shortest_process->remaining_time) {
            // 동점 시 도착시간 기준
            if (scheduler->ready_queue[i]->arrival_time < shortest_process->arrival_time) {
                shortest_process = scheduler->ready_queue[i];
                shortest_process_idx = i;
            } else if (scheduler->ready_queue[i]->arrival_time == shortest_process->arrival_time) {
                // 도착시간도 동일하면 프로세스 ID가 더 작은 것 선택
                if (scheduler->ready_queue[i]->pid < shortest_process->pid) {
                    shortest_process = scheduler->ready_queue[i];
                    shortest_process_idx = i;
                }
            }
        }
    }
    
    if (current_process != NULL) {
        if (current_process->remaining_time <= shortest_process->remaining_time) {
            return NULL;  // 현재 실행 중인 프로세스가 더 짧거나 같으면 선점하지 않음
        }
    }
    
    remove_from_ready_queue(scheduler, shortest_process_idx);
    return shortest_process;
}

Process* select_highest_process(Scheduler* scheduler, Process* current_process) {
    if (scheduler->ready_queue_cnt <= 0) {
        return NULL;
    }

    Process* highest_process = scheduler->ready_queue[0];
    int highest_process_idx = 0;

    for (int i=1; i<scheduler->ready_queue_cnt; i++) {
        if (scheduler->ready_queue[i]->priority < highest_process->priority) {
            highest_process = scheduler->ready_queue[i];
            highest_process_idx = i;
        } else if (scheduler->ready_queue[i]->priority == highest_process->priority) {
            // 동점 시 도착시간 기준
            if (scheduler->ready_queue[i]->arrival_time < highest_process->arrival_time) {
                highest_process = scheduler->ready_queue[i];
                highest_process_idx = i;
            } else if (scheduler->ready_queue[i]->arrival_time == highest_process->arrival_time) {
                // 도착시간도 동일하면 프로세스 ID가 더 작은 것 선택
                if (scheduler->ready_queue[i]->pid < highest_process->pid) {
                    highest_process = scheduler->ready_queue[i];
                    highest_process_idx = i;
                }
            }
        }
    }
    remove_from_ready_queue(scheduler, highest_process_idx);
    return highest_process;
}


// 프로세스를 실행하는 것이 아니라 시작 환경을 세팅하는 것
void start_process(Process* process, GanttChart* chart_item, int* is_chart_item_initialized, int current_simulation_time) {
    process->state = RUNNING;

    if(process->is_first_execution) {
        process->response_time = current_simulation_time;
        process->is_first_execution = false;
    }
    
    chart_item->start_time = current_simulation_time;
    sprintf(chart_item->process_name, "P%d", process->pid);
    *is_chart_item_initialized = 1;
}

// 프로세스를 실제로 실행 처리함 (i/o 작업 처리 포함)
void execute_process(Process** current_process, Scheduler* scheduler, GanttChart* chart_item, int* is_chart_item_initialized, int current_simulation_time, int * terminated_process_cnt) {
    
    (*current_process)->remaining_time -= 1;
    printf("P%d가 실행되었습니다. (현재시간: %d)\n", (*current_process)->pid, current_simulation_time);

    int is_io_request_exists = false;
    handle_io_task_of_process(*current_process, scheduler, &is_io_request_exists);

    if (is_io_request_exists) {
        if (*is_chart_item_initialized == 1) {
            chart_item->end_time = current_simulation_time + 1;
            scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = *chart_item;
            *is_chart_item_initialized = 0;
        }
        *current_process = NULL;
    // 종료 처리
    } else if ((*current_process)->remaining_time == 0) {
        (*current_process)->state = TERMINATED;
        (*terminated_process_cnt)++;

        // GanttChart 생성용
        if (*is_chart_item_initialized == 1) {
            chart_item->end_time = current_simulation_time + 1;
            scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = *chart_item;
            *is_chart_item_initialized = 0;

            // SECTION - 성능측정
            (*current_process)->completion_time = current_simulation_time + 1;
            (*current_process)->turnaround_time = (*current_process)->completion_time - (*current_process)->arrival_time;
        }
        
        // ready queue에서 제거
        for (int i = 0; i < scheduler->ready_queue_cnt; i++) {
            if (scheduler->ready_queue[i]->pid == (*current_process)->pid) {
                remove_from_ready_queue(scheduler, i);
                break;
            }
        }
        
        printf("P%d가 종료되었습니다. (현재시간: %d)\n", (*current_process)->pid, current_simulation_time);
        *current_process = NULL;
    }
}

void handle_gantt_chart_idle(Scheduler* scheduler, int* is_idle, GanttChart* idle_item, int current_simulation_time) {
    if (!(*is_idle)) {
        idle_item->start_time = current_simulation_time;
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

void update_waiting_time(Scheduler* scheduler, int current_time) {
    printf("\n=== 대기 시간 업데이트 (현재 시간: %d) ===\n", current_time);
    printf("Ready Queue의 프로세스들:\n");
    
    for (int i=0; i<scheduler->ready_queue_cnt; i++) {
        scheduler->ready_queue[i]->waiting_time += 1;
        printf("P%d - 대기 시간: %d\n", 
            scheduler->ready_queue[i]->pid,
            scheduler->ready_queue[i]->waiting_time);
    }
    printf("===========================\n");
}