#ifndef PROCESS_H
#define PROCESS_H
#include <stdbool.h>

#define MAX_IO_COUNT 3

typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct {
    int pid; 
    int arrival_time; 
    int cpu_burst_time;
    int remaining_time; // 남은 실행시간 관리
    int priority;

    ProcessState state;

    /* 성능 지표용 변수 */
    int waiting_time; 
    int turnaround_time;
    int response_time;

    /* IO 작업 처리 */
    int io_count; // 전체 io 작업횟수 (최대3번)
    int io_request_times[MAX_IO_COUNT]; // TODO - IO 요청시점 중복처리할지?
    int io_burst_times[MAX_IO_COUNT];
    int current_io_idx;
    int io_remaining_time; // 현재 처리되는 io작업 기준임
    bool is_doing_io;
} Process;

Process * create_process(int arrival_time, int cpu_burst_time, int priority);
void destroy_process(Process * process);
void print_process(Process * process);
void sort_io_requests(Process* process);

#endif