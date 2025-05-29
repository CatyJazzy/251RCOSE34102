#ifndef UTILS_H
#define UTILS_H

#include "scheduler.h"

// 도착한 프로세스를 확인하여 ready queue에 추가
void update_arrivals(Scheduler* scheduler, int current_time);

// waiting queue에 있는 I/O작업들을 처리함 (아래 함수랑 다름!)
void process_io_operations(Scheduler* scheduler, int* terminated_process_cnt);

// 현재 실행 중인 프로세스의 I/O 요청을 처리함
void handle_io_task_of_process(Process* process, Scheduler* scheduler, int* is_moved_to_waiting);

// ready queue에서 프로세스 제거
void remove_from_ready_queue(Scheduler* scheduler, int idx);

void update_idle_state(Scheduler* scheduler, int* is_idle, GanttChart* idle_item, int current_simulation_time, Process* current_process);
void handle_gantt_chart_idle(Scheduler* scheduler, int* is_idle, GanttChart* idle_item, int current_simulation_time);
void end_gantt_chart_idle(Scheduler* scheduler, int* is_idle, GanttChart* idle_item, int current_simulation_time);
void print_scheduling_debug_info(Scheduler* scheduler, Process* current_process, int current_time); 
bool is_new_process_need(Scheduler* scheduler, Process* current_process);
Process* select_shortest_process(Scheduler* scheduler, Process* current_process);
Process* select_shortest_remaining_process(Scheduler* scheduler, Process* current_process);
Process* select_highest_process(Scheduler* scheduler, Process* current_process);
Process* select_earlier_process(Scheduler* scheduler, Process* current_process);
void start_process(Process* process, GanttChart* chart_item, int* is_chart_item_initialized, int current_simulation_time);
void execute_process(Process** current_process, Scheduler* scheduler, GanttChart* chart_item, int* is_chart_item_initialized, int current_simulation_time, int * terminated_process_cnt);
void update_waiting_time(Scheduler* scheduler);

#endif 