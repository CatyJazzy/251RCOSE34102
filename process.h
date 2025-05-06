#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

// 프로세스 상태를 나타내는 열거형
typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

// 프로세스 구조체
typedef struct {
    int pid;                // 프로세스 ID
    int arrival_time;       // 도착 시간
    int cpu_burst_time;     // CPU 버스트 시간
    int remaining_time;     // 남은 실행 시간
    int io_burst_time;      // I/O 버스트 시간
    int io_request_time;    // I/O 요청 시간
    int priority;           // 우선순위
    int waiting_time;       // 대기 시간
    int turnaround_time;    // 반환 시간
    ProcessState state;     // 프로세스 상태
    bool is_io_completed;   // I/O 완료 여부
} Process;

// 프로세스 관련 함수 선언
Process* create_process(int pid, int arrival_time, int cpu_burst_time, 
                       int io_burst_time, int io_request_time, int priority);
void destroy_process(Process* process);
void print_process_info(Process* process);

#endif // PROCESS_H 