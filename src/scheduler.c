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
        scheduler->process_arr[i] = create_process(i + 1, 0, 0, 0);  
        printf("%d번째 프로세스의 도착 시간을 입력하세요 : ", i+1);
        scanf("%d", &scheduler->process_arr[i]->arrival_time);
        printf("%d번째 프로세스의 CPU 실행 시간을 입력하세요 : ", i+1);
        scanf("%d", &scheduler->process_arr[i]->cpu_burst_time);
        scheduler->process_arr[i]->remaining_time = scheduler->process_arr[i]->cpu_burst_time;  

        if (scheduler->process_arr[i]->cpu_burst_time < 1) {
            printf("CPU 실행 시간은 1 이상이어야 합니다. 기본값으로 1로 설정됩니다.\n");
            scheduler->process_arr[i]->cpu_burst_time = 1;
        } 
        if (scheduler->process_arr[i]->cpu_burst_time > 10) {
            printf("CPU 실행 시간은 10 이하이어야 합니다. 기본값으로 10으로 설정됩니다.\n");
            scheduler->process_arr[i]->cpu_burst_time = 10;
        }

        printf("%d번째 프로세스의 우선순위를 입력하세요 : ", i+1);
        scanf("%d", &scheduler->process_arr[i]->priority);
        printf("\n");
    }

    for (int i=0; i<process_cnt; i++) {
        // cpu_burst_time이 1일 때는 요청 발생하지 않도록 함
        if (scheduler->process_arr[i]->cpu_burst_time == 1) {
            scheduler->process_arr[i]->io_count = 0; 
            continue;
        }  

        int max_possible_io_items = scheduler->process_arr[i]->cpu_burst_time - 1;

        // 1 ~ min(cpu_burst_time, 3) 범위에서 랜덤 = IO 요청 개수
        int max_io_frequency = (max_possible_io_items < MAX_IO_COUNT) ? 
    max_possible_io_items : MAX_IO_COUNT;
    
        scheduler->process_arr[i]->io_count = rand() % max_io_frequency + 1;

        int possible_times[max_possible_io_items];
        for (int t=1; t<scheduler->process_arr[i]->cpu_burst_time; t++) {
            possible_times[t-1] = t;
        }

        int remaining_count = max_possible_io_items;
        for (int j=0; j<scheduler->process_arr[i]->io_count; j++) {
            if (remaining_count <= 0) break;

            int random_index = rand() % remaining_count;
            scheduler->process_arr[i]->io_request_times[j] = possible_times[random_index];

            // 선택된 인덱스의 값을 배열의 마지막으로 이동하고 remaining_count 감소
            int temp = possible_times[random_index];
            possible_times[random_index] = possible_times[remaining_count - 1];
            possible_times[remaining_count - 1] = temp;
            remaining_count--;

            // [버스트 시간] 1~3시간 중 랜덤으로 설정
            scheduler->process_arr[i]->io_burst_times[j] = rand() % 3 + 1;
        }

        // IO 요청 시점 정렬
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

        update_arrivals(scheduler, current_simulation_time);

        update_idle_state(scheduler, &is_idle, &idle_item, current_simulation_time, current_process);

        print_scheduling_debug_info(scheduler, current_process, current_simulation_time);

        if (is_new_process_need(scheduler, current_process)) {
            Process* shortest_process = select_shortest_process(scheduler, current_process);
            
            if (shortest_process != NULL) {
                start_process(shortest_process, &chart_item, &is_chart_item_initialized, current_simulation_time);
                current_process = shortest_process;
            }
        }

        process_io_operations(scheduler, &terminated_process_cnt);

        if (current_process != NULL) {
            execute_process(&current_process, scheduler, &chart_item, &is_chart_item_initialized, current_simulation_time, &terminated_process_cnt);
        }
        current_simulation_time++;   
    }
    end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
}
void schedule_sjf_p(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;

    int is_idle = 0;
    GanttChart idle_item;

    Process* current_process = NULL;
    GanttChart chart_item;
    int is_chart_item_initialized = 0;

    while(terminated_process_cnt < scheduler->process_cnt) {    
        // 도착한 프로세스 확인함
        update_arrivals(scheduler, current_simulation_time);

        update_idle_state(scheduler, &is_idle, &idle_item, current_simulation_time, current_process);

        print_scheduling_debug_info(scheduler, current_process, current_simulation_time);
        
        Process * shortest_process = select_shortest_remaining_process(scheduler, current_process);

        // 선점될 때 처리
        if (shortest_process != NULL) {
            if(current_process) {
                current_process->state = READY;
                scheduler->ready_queue[scheduler->ready_queue_cnt++] = current_process;

                if(is_chart_item_initialized) {
                    chart_item.end_time = current_simulation_time;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                    is_chart_item_initialized = 0;
                } 
            }

            start_process(shortest_process, &chart_item, &is_chart_item_initialized, current_simulation_time);
            current_process = shortest_process;
        }

        process_io_operations(scheduler, &terminated_process_cnt);

        if (current_process != NULL) {
            execute_process(&current_process, scheduler, &chart_item, &is_chart_item_initialized, current_simulation_time, &terminated_process_cnt);
        }
        current_simulation_time++;
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
        // 도착한 프로세스 확인함
        update_arrivals(scheduler, current_simulation_time);
        update_idle_state(scheduler, &is_idle, &idle_item, current_simulation_time, current_process);

        print_scheduling_debug_info(scheduler, current_process, current_simulation_time);

        if (is_new_process_need(scheduler, current_process)) {
            Process* shortest_process = select_earlier_process(scheduler, current_process);
            
            if (shortest_process != NULL) {
                start_process(shortest_process, &chart_item, &is_chart_item_initialized, current_simulation_time);
                current_process = shortest_process;
            }
        }

        process_io_operations(scheduler, &terminated_process_cnt);
        
        if (current_process != NULL) {
            execute_process(&current_process, scheduler, &chart_item, &is_chart_item_initialized, current_simulation_time, &terminated_process_cnt);
        } 

        current_simulation_time++;
    }
    end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
}
void schedule_priority_p(Scheduler* scheduler) {
    int current_simulation_time = 0;
    int terminated_process_cnt = 0;

    GanttChart idle_item;
    int is_idle = 0;

    Process* current_process = NULL;
    GanttChart chart_item;
    int is_chart_item_initialized = 0;

    while (terminated_process_cnt < scheduler->process_cnt) {
        update_arrivals(scheduler, current_simulation_time);
        
        update_idle_state(scheduler, &is_idle, &idle_item, current_simulation_time, current_process);

        print_scheduling_debug_info(scheduler, current_process, current_simulation_time);

        Process* highest_process = select_highest_process(scheduler, current_process);

        // 선점될 때 처리
        if (highest_process != NULL) {
            if(current_process) {
                current_process->state = READY;
                scheduler->ready_queue[scheduler->ready_queue_cnt++] = current_process;

                if(is_chart_item_initialized) {
                    chart_item.end_time = current_simulation_time;
                    scheduler->gantt_chart[scheduler->gantt_chart_cnt++] = chart_item;
                    is_chart_item_initialized = 0;
                } 
            }

            start_process(highest_process, &chart_item, &is_chart_item_initialized, current_simulation_time);
            current_process = highest_process;
        }

        process_io_operations(scheduler, &terminated_process_cnt);

        if (current_process != NULL) {
            execute_process(&current_process, scheduler, &chart_item, &is_chart_item_initialized, current_simulation_time, &terminated_process_cnt);
        }
        current_simulation_time++;
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
        update_arrivals(scheduler, current_simulation_time);
        
        update_idle_state(scheduler, &is_idle, &idle_item, current_simulation_time, current_process);

        // 디버깅 정보 출력
        print_scheduling_debug_info(scheduler, current_process, current_simulation_time);

        if (is_new_process_need(scheduler, current_process)) {
            Process* highest_process = select_highest_process(scheduler, current_process);
            
            if (highest_process != NULL) {
                start_process(highest_process, &chart_item, &is_chart_item_initialized, current_simulation_time);
                current_process = highest_process;
            }
        }   

        process_io_operations(scheduler, &terminated_process_cnt);

        if (current_process != NULL) {
            execute_process(&current_process, scheduler, &chart_item, &is_chart_item_initialized, current_simulation_time, &terminated_process_cnt);
        }
        current_simulation_time++;  
    }
    end_gantt_chart_idle(scheduler, &is_idle, &idle_item, current_simulation_time);
}