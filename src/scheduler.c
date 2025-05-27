#include "scheduler.h"
#include "utils.h"
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
    scheduler->gantt_chart_cnt = 0;

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
        // CPU 실행 시간이 1인 경우 IO 요청 없이 예외처리
        if (scheduler->process_arr[i]->cpu_burst_time <= 1) {
            scheduler->process_arr[i]->io_count = 0;
        } else {
            for (int j=0; j<scheduler->process_arr[i]->io_count; j++) {
                scheduler->process_arr[i]->io_request_times[j] = (rand() % (scheduler->process_arr[i]->cpu_burst_time - 1)) + 1;
            }
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
            schedule_sjf_p(scheduler);
            break;
        case PRIORITY_NP:
            schedule_priority_np(scheduler);
            break;
        case PRIORITY_P:
            schedule_priority_p(scheduler);
            break;
        case ROUND_ROBIN:
            break;
        default:
            break;
    }   
    return scheduler;
}

void Evaluation(Scheduler* scheduler) {
    printf("\n\n|---- 성능 측정 결과 ----|\n");
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    for (int i=0; i<scheduler->process_cnt; i++) {
        total_waiting_time += scheduler->process_arr[i]->waiting_time;
        total_turnaround_time += scheduler->process_arr[i]->turnaround_time;
    }
    printf("평균 Waiting Time: %.2f\n", (float)total_waiting_time / scheduler->process_cnt);
    printf("평균 Turnaround Time: %.2f\n", (float)total_turnaround_time / scheduler->process_cnt);
}

void schedule_sjf_np(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;
    Process* current_process = NULL;

    GanttChart idle_item;
    int is_idle = 0;

    // GanttChart 생성용
    GanttChart chart_item;
    int is_chart_item_initialized = 0;
    

    while(terminated_process_cnt < scheduler->process_cnt) {

        if (scheduler->ready_queue_cnt == 0) {
           handle_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        } else {
            end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        }

        // 도착한 프로세스 확인함
        check_and_add_arrived_processes(scheduler, current_simulation_time);
        process_io_operations(scheduler, &terminated_process_cnt);

        if (current_process == NULL && scheduler->ready_queue_cnt > 0) {
           Process* shortest_process = scheduler->ready_queue[0]; // 임시 설정
           int shortest_process_idx = 0;

            for (int i=0; i<scheduler->ready_queue_cnt; i++) {
                if (scheduler->ready_queue[i]->cpu_burst_time < shortest_process->cpu_burst_time) {
                    shortest_process = scheduler->ready_queue[i]; // Shorted Job 프로세스 선택
                    shortest_process_idx = i;
                } else if (scheduler->ready_queue[i]->cpu_burst_time == shortest_process->cpu_burst_time) {
                    // 동점 시 도착시간 기준
                    if (scheduler->ready_queue[i]->arrival_time < shortest_process->arrival_time) {
                        shortest_process = scheduler->ready_queue[i];
                        shortest_process_idx = i;
                    }
                }
            }

            chart_item.start_time = current_simulation_time;
            sprintf(chart_item.process_name, "P%d", shortest_process->pid);
            is_chart_item_initialized = 1;

            current_process = shortest_process;
            current_process->state = RUNNING;
            remove_from_ready_queue(scheduler, shortest_process_idx);
            
            //SECTION - 성능측정
            if (current_process->is_first_execution) {
                current_process->response_time = current_simulation_time;
                current_process->is_first_execution = false;
            }
        }

        if (current_process != NULL) {
            current_process->remaining_time -= 1; // CPU 실행
            printf("P%d가 실행되었습니다. (현재시간: %d)\n", current_process->pid, current_simulation_time);

            int is_moved_to_waiting = false;
            handle_io_task_of_process(current_process, scheduler, &is_moved_to_waiting);

            if (is_moved_to_waiting) {
                if (is_chart_item_initialized == 1) {
                    chart_item.end_time = current_simulation_time + 1;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                    is_chart_item_initialized = 0;
                }
                current_process = NULL;
            } else if (current_process ->remaining_time <= 0) {
                current_process->state = TERMINATED;
                terminated_process_cnt++;

                // GanttChart 생성용
                if (is_chart_item_initialized == 1) {
                    chart_item.end_time = current_simulation_time + 1;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                    is_chart_item_initialized = 0;
                }
                
                //SECTION - 성능측정
                current_process->completion_time = current_simulation_time + 1;
                current_process->turnaround_time = current_process->completion_time - current_process->arrival_time;
                current_process->waiting_time = current_process->turnaround_time - current_process->cpu_burst_time;
                
                printf("P%d가 종료되었습니다. (현재시간: %d)\n", current_process->pid, current_simulation_time);
                current_process = NULL;
            }
        }
        printf("현재 시간: %d\n", current_simulation_time);
        current_simulation_time++;   
    }

    end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
}
void schedule_sjf_p(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;

    int is_idle = 0;
    GanttChart idle_item;

    Process* prev_process = NULL;
    GanttChart chart_item;

    while(terminated_process_cnt < scheduler->process_cnt) {

        if (scheduler->ready_queue_cnt == 0) {
           handle_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        } else {
            end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        }
        
        // 도착한 프로세스 확인함
        check_and_add_arrived_processes(scheduler, current_simulation_time);
        process_io_operations(scheduler, &terminated_process_cnt);
        
        if (scheduler->ready_queue_cnt > 0) {
            Process* current_process = scheduler->ready_queue[0]; // 임시 설정
            int current_process_idx = 0;

            for (int i=0; i<scheduler->ready_queue_cnt; i++) {

                if (scheduler->ready_queue[i]->state == TERMINATED) {
                    continue;
                }

                if (scheduler->ready_queue[i]->remaining_time < current_process->remaining_time) {
                    current_process = scheduler->ready_queue[i]; // Shorted Job 프로세스 선택
                    current_process_idx = i;
                } else if (scheduler->ready_queue[i]->remaining_time == current_process->remaining_time) {
                    if (scheduler->ready_queue[i]->arrival_time < current_process->arrival_time) {
                        current_process = scheduler->ready_queue[i];
                        current_process_idx = i;
                    }
                }
            }

            if (prev_process != current_process) {
                if(prev_process != NULL) {
                    // 이전 프로세스 실행내역 기록
                    chart_item.end_time = current_simulation_time;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                }

                // 업데이트
                chart_item.start_time = current_simulation_time;
                sprintf(chart_item.process_name, "P%d", current_process->pid);
                prev_process = current_process;
            }

            current_process->state = RUNNING;
            current_process->remaining_time -= 1; 

            printf("P%d가 실행되었습니다. (현재시간: %d)\n", current_process->pid, current_simulation_time);

            //SECTION - 성능측정
            if (current_process->is_first_execution) {
                current_process->response_time = current_simulation_time;
                current_process->is_first_execution = false;
            }

             int is_moved_to_waiting = false;
            handle_io_task_of_process(current_process, scheduler, &is_moved_to_waiting);

            if (is_moved_to_waiting) {
                // ready-Q 제거
                chart_item.end_time = current_simulation_time + 1;
                scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                remove_from_ready_queue(scheduler, current_process_idx);
                prev_process = NULL;
            } else if (current_process ->remaining_time <= 0 &&current_process->is_doing_io == false) {
                current_process->state = TERMINATED;
                terminated_process_cnt++;

                chart_item.end_time = current_simulation_time + 1;
                scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                prev_process = NULL;

                //SECTION - 성능측정
                current_process->completion_time = current_simulation_time + 1;
                current_process->turnaround_time = current_process->completion_time - current_process->arrival_time;
                current_process->waiting_time = current_process->turnaround_time - current_process->cpu_burst_time;
                
                printf("P%d가 종료되었습니다. (현재시간: %d)\n", current_process->pid, current_simulation_time);
                remove_from_ready_queue(scheduler, current_process_idx);
            }
        } else {
            // ready-Q 비어있을 때 초기화
            if (prev_process != NULL) {
                chart_item.end_time = current_simulation_time;
                scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                prev_process = NULL;
            }
        }
        printf("현재 시간: %d\n", current_simulation_time);
        current_simulation_time++;
    }

    // 마지막 남은 구간 처리
    if (prev_process != NULL) {
        chart_item.end_time = current_simulation_time;
        scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
    }
    end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
}
void schedule_fcfs(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;
    Process* current_process = NULL;

    GanttChart idle_item;
    int is_idle = 0;

    GanttChart chart_item;
    int is_chart_item_initialized = 0;

    while (terminated_process_cnt < scheduler->process_cnt) {
         if (scheduler->ready_queue_cnt == 0) {
           handle_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        } else {
            end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        }

        // 도착한 프로세스 확인함
        check_and_add_arrived_processes(scheduler, current_simulation_time);
        process_io_operations(scheduler, &terminated_process_cnt);

        if (current_process == NULL && scheduler->ready_queue_cnt > 0) {
            current_process = scheduler->ready_queue[0]; // FCFS이므로 맨 앞에 있는 프로세스 선택
            current_process->state = RUNNING;

            chart_item.start_time = current_simulation_time;
            sprintf(chart_item.process_name, "P%d", current_process->pid);
            is_chart_item_initialized = 1;

            remove_from_ready_queue(scheduler, 0);

            //SECTION - 성능측정
            if (current_process->is_first_execution) {
                current_process->response_time = current_simulation_time;
                current_process->is_first_execution = false;
            }
        }
        
        if (current_process != NULL) {
            current_process->remaining_time -= 1; // CPU 실행 처리
            printf("P%d가 실행되었습니다. (현재시간: %d)\n", current_process->pid, current_simulation_time);

            int is_moved_to_waiting = false;
            handle_io_task_of_process(current_process, scheduler, &is_moved_to_waiting);

            if (is_moved_to_waiting) {
                if (is_chart_item_initialized == 1) {
                    chart_item.end_time = current_simulation_time + 1;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                    is_chart_item_initialized = 0;
                }
               current_process = NULL;
            } else if (current_process->remaining_time <= 0) {
                current_process->state = TERMINATED;
                terminated_process_cnt++;

                if (is_chart_item_initialized == 1) {
                    chart_item.end_time = current_simulation_time + 1;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                    is_chart_item_initialized = 0;
                }

                //SECTION - 성능측정
                current_process->completion_time = current_simulation_time + 1;
                current_process->turnaround_time = current_process->completion_time - current_process->arrival_time;
                current_process->waiting_time = current_process->turnaround_time - current_process->cpu_burst_time;

                printf("P%d가 종료되었습니다. (현재시간: %d)\n", current_process->pid, current_simulation_time);
                current_process = NULL;
            }
        } 

        current_simulation_time++;
        printf("현재 시간: %d\n", current_simulation_time);
    }
    end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
}
void schedule_priority_p(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;

    GanttChart idle_item;
    int is_idle = 0;

    Process* prev_process = NULL;
    GanttChart chart_item;

    while (terminated_process_cnt < scheduler->process_cnt) {
        if (scheduler->ready_queue_cnt == 0) {
           handle_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        } else {
            end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        }
        check_and_add_arrived_processes(scheduler, current_simulation_time);
        process_io_operations(scheduler, &terminated_process_cnt);

        if (scheduler->ready_queue_cnt > 0) {
            Process* process = scheduler->ready_queue[0]; // 임시 설정
            int process_idx = 0;  

            for (int i=0; i<scheduler->ready_queue_cnt; i++) {

                if (scheduler->ready_queue[i]->state == TERMINATED) continue;

                // NOTE - priority 작을수록 중요도가 높음 (동점일 때는 도착시간 더 빠른쪽이 우선!)
                if(scheduler->ready_queue[i]->priority <= process->priority) {
                    bool is_more_important = (scheduler->ready_queue[i]->priority == process->priority && 
     scheduler->ready_queue[i]->arrival_time < process->arrival_time); // 동점처리 boolean
                    if (scheduler->ready_queue[i]->priority < process->priority || is_more_important) {
                        process = scheduler->ready_queue[i];
                        process_idx = i;  
                    }
                }
            }

             if (prev_process != process) {
                if(prev_process != NULL) {
                    // 이전 프로세스 실행내역 기록
                    chart_item.end_time = current_simulation_time;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                }

                // 업데이트
                chart_item.start_time = current_simulation_time;
                sprintf(chart_item.process_name, "P%d", process->pid);
                prev_process = process;
            }

            process->state = RUNNING;
            process->remaining_time -= 1;

            printf("P%d가 실행되었습니다. (현재시간: %d)\n", process->pid, current_simulation_time);

            //SECTION - 성능측정
            if (process->is_first_execution) {
                process->response_time = current_simulation_time;
                process->is_first_execution = false;
            }
        

            int is_moved_to_waiting = false;
            handle_io_task_of_process(process, scheduler, &is_moved_to_waiting);

            if (is_moved_to_waiting) {
                chart_item.end_time = current_simulation_time + 1;
                scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                remove_from_ready_queue(scheduler, process_idx);  
                prev_process = NULL;
            } else if (process ->remaining_time <= 0 &&process->is_doing_io == false) {
                process->state = TERMINATED;
                terminated_process_cnt++;

                // GanttChart 생성용
                chart_item.end_time = current_simulation_time + 1;
                scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                prev_process = NULL;
                
                //SECTION - 성능측정
                process->completion_time = current_simulation_time + 1;
                process->turnaround_time = process->completion_time - process->arrival_time;
                process->waiting_time = process->turnaround_time - process->cpu_burst_time;
                
                printf("P%d가 종료되었습니다.(현재시간: %d)\n", process->pid, current_simulation_time);
                remove_from_ready_queue(scheduler, process_idx);
            }
        } else {
            // ready-Q 비어있을 때 초기화
            if (prev_process != NULL) {
                chart_item.end_time = current_simulation_time;
                scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                prev_process = NULL;
            }
        }
        printf("현재 시간: %d\n", current_simulation_time);
        current_simulation_time++;
    }
    // 마지막 남은 구간 처리
    if (prev_process != NULL) {
        chart_item.end_time = current_simulation_time;
        scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
    }
    end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
}
void schedule_priority_np(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;
    Process* current_process = NULL;

    GanttChart idle_item;
    int is_idle = 0;

    GanttChart chart_item;
    int is_chart_item_initialized = 0;

    while (terminated_process_cnt < scheduler->process_cnt) {
         if (scheduler->ready_queue_cnt == 0) {
           handle_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        } else {
            end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
        }
        check_and_add_arrived_processes(scheduler, current_simulation_time);
        process_io_operations(scheduler, &terminated_process_cnt);

        if (current_process == NULL && scheduler->ready_queue_cnt > 0) {
            Process* highest_priority_process = scheduler->ready_queue[0];
            int highest_priority_process_idx = 0;

            for (int i=0; i<scheduler->ready_queue_cnt; i++) {
                // NOTE - priority 작을수록 중요도가 높음 (동점일 때는 도착시간 더 빠른쪽이 우선!)
                if(scheduler->ready_queue[i]->priority <= highest_priority_process->priority) {
                    bool is_more_important = (scheduler->ready_queue[i]->priority == highest_priority_process->priority && 
     scheduler->ready_queue[i]->arrival_time < highest_priority_process->arrival_time); // 동점처리 boolean
                    if (scheduler->ready_queue[i]->priority < highest_priority_process->priority || is_more_important) {
                        highest_priority_process = scheduler->ready_queue[i];
                        highest_priority_process_idx = i;  
                    }
                }
            }
            current_process = highest_priority_process;
            current_process->state = RUNNING;

            chart_item.start_time = current_simulation_time;
            sprintf(chart_item.process_name, "P%d", current_process->pid);
            is_chart_item_initialized = 1;
            
            //SECTION - 성능측정
            if (current_process->is_first_execution) {
                current_process->response_time = current_simulation_time;
                current_process->is_first_execution = false;
            }
            
            remove_from_ready_queue(scheduler, highest_priority_process_idx);
        }

        if (current_process != NULL) {
            current_process->remaining_time -= 1; 
            printf("P%d가 실행되었습니다. (현재시간: %d)\n", current_process->pid, current_simulation_time);

            int is_moved_to_waiting = false;
            handle_io_task_of_process(current_process, scheduler, &is_moved_to_waiting);

            if (is_moved_to_waiting) {
                if (is_chart_item_initialized == 1) {
                    chart_item.end_time = current_simulation_time + 1;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                    is_chart_item_initialized = 0;
                }
                current_process = NULL;
            } else if (current_process ->remaining_time <= 0) {
                current_process->state = TERMINATED;
                terminated_process_cnt++;

                // GanttChart 생성용
                if (is_chart_item_initialized == 1) {
                    chart_item.end_time = current_simulation_time + 1;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                    is_chart_item_initialized = 0;
                }
                
                //SECTION - 성능측정
                current_process->completion_time = current_simulation_time + 1;
                current_process->turnaround_time = current_process->completion_time - current_process->arrival_time;
                current_process->waiting_time = current_process->turnaround_time - current_process->cpu_burst_time;
                
                printf("P%d가 종료되었습니다. (현재시간: %d)\n", current_process->pid, current_simulation_time);
                current_process = NULL;
            }
        }
        printf("현재 시간: %d\n", current_simulation_time);
        current_simulation_time++;
    }
    end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
}