#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "process.h"
#include "scheduler.h"

#define MAX_PROCESSES 100
#define MAX_TIME INT_MAX / 2  // 시간값의 최대값 제한
#define MAX_RETRIES 3  // 최대 재시도 횟수

// 입력 버퍼 초기화 함수
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 입력 취소 여부 확인
int is_cancel_input(const char* input) {
    return (strcmp(input, "q\n") == 0 || strcmp(input, "Q\n") == 0);
}

// 안전한 정수 입력 함수
int safe_input_int(int* value, int min, int max, int* retry_count) {
    char input[32];
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return -1;  // 입력 실패
    }
    
    // 입력 취소 확인
    if (is_cancel_input(input)) {
        return -2;  // 입력 취소
    }
    
    // 입력 버퍼 오버플로우 체크
    if (strchr(input, '\n') == NULL) {
        clear_input_buffer();
        (*retry_count)++;
        return 0;
    }
    
    char* endptr;
    long num = strtol(input, &endptr, 10);
    
    // 숫자가 아닌 문자가 있는지 체크
    if (*endptr != '\n') {
        (*retry_count)++;
        return 0;
    }
    
    // 범위 검사
    if (num < min || num > max) {
        (*retry_count)++;
        return 0;
    }
    
    *value = (int)num;
    return 1;
}

// 입력 재시도 처리
int handle_input_retry(int* retry_count, const char* field_name, int min, int max) {
    if (*retry_count >= MAX_RETRIES) {
        printf("\n최대 재시도 횟수(%d회)를 초과했습니다.\n", MAX_RETRIES);
        printf("프로그램을 종료합니다.\n");
        return 0;
    }
    
    printf("오류: 올바른 숫자를 입력해주세요 (%s: %d-%d).\n", field_name, min, max);
    printf("다시 시도하시려면 숫자를, 종료하시려면 'q'를 입력하세요.\n");
    printf("남은 시도 횟수: %d\n", MAX_RETRIES - *retry_count);
    return 1;
}

// 유효성 검증 함수
int validate_process_input(int arrival_time, int cpu_burst_time, 
                         int io_burst_time, int io_request_time, int priority,
                         Process** processes, int current_index) {
    // 기본 검증
    if (arrival_time < 0) {
        printf("오류: 도착 시간은 음수가 될 수 없습니다.\n");
        return 0;
    }
    if (cpu_burst_time <= 0) {
        printf("오류: CPU 버스트 시간은 0보다 커야 합니다.\n");
        return 0;
    }
    if (io_burst_time < 0) {
        printf("오류: I/O 버스트 시간은 음수가 될 수 없습니다.\n");
        return 0;
    }
    if (io_request_time < arrival_time) {
        printf("오류: I/O 요청 시간은 도착 시간보다 이후여야 합니다.\n");
        return 0;
    }
    if (io_request_time >= cpu_burst_time) {
        printf("오류: I/O 요청 시간은 CPU 버스트 시간보다 작아야 합니다.\n");
        return 0;
    }
    if (priority <= 0) {
        printf("오류: 우선순위는 0보다 커야 합니다.\n");
        return 0;
    }
    
    // 최대값 검증
    if (arrival_time > MAX_TIME || cpu_burst_time > MAX_TIME || 
        io_burst_time > MAX_TIME || io_request_time > MAX_TIME) {
        printf("오류: 입력값이 너무 큽니다. 최대값은 %d입니다.\n", MAX_TIME);
        return 0;
    }
    
    // 프로세스 간 시간 충돌 검사
    for (int i = 0; i < current_index; i++) {
        if (processes[i]->arrival_time == arrival_time) {
            printf("오류: 이미 같은 도착 시간을 가진 프로세스가 존재합니다.\n");
            return 0;
        }
    }
    
    return 1;
}

Process* input_process(int pid, Process** processes, int current_index) {
    int arrival_time, cpu_burst_time, io_burst_time, io_request_time, priority;
    int retry_count = 0;
    
    while (1) {
        printf("\n프로세스 %d 정보 입력\n", pid);
        printf("(입력 취소: 'q' 또는 'Q' 입력)\n");
        
        printf("도착 시간: ");
        int result = safe_input_int(&arrival_time, 0, MAX_TIME, &retry_count);
        if (result == -2) return NULL;  // 입력 취소
        if (result <= 0) {
            if (!handle_input_retry(&retry_count, "도착 시간", 0, MAX_TIME)) return NULL;
            continue;
        }
        
        printf("CPU 버스트 시간: ");
        result = safe_input_int(&cpu_burst_time, 1, MAX_TIME, &retry_count);
        if (result == -2) return NULL;
        if (result <= 0) {
            if (!handle_input_retry(&retry_count, "CPU 버스트 시간", 1, MAX_TIME)) return NULL;
            continue;
        }
        
        printf("I/O 버스트 시간: ");
        result = safe_input_int(&io_burst_time, 0, MAX_TIME, &retry_count);
        if (result == -2) return NULL;
        if (result <= 0) {
            if (!handle_input_retry(&retry_count, "I/O 버스트 시간", 0, MAX_TIME)) return NULL;
            continue;
        }
        
        printf("I/O 요청 시간: ");
        result = safe_input_int(&io_request_time, arrival_time, cpu_burst_time - 1, &retry_count);
        if (result == -2) return NULL;
        if (result <= 0) {
            if (!handle_input_retry(&retry_count, "I/O 요청 시간", arrival_time, cpu_burst_time - 1)) return NULL;
            continue;
        }
        
        printf("우선순위: ");
        result = safe_input_int(&priority, 1, INT_MAX, &retry_count);
        if (result == -2) return NULL;
        if (result <= 0) {
            if (!handle_input_retry(&retry_count, "우선순위", 1, INT_MAX)) return NULL;
            continue;
        }
        
        if (validate_process_input(arrival_time, cpu_burst_time, 
                                 io_burst_time, io_request_time, priority,
                                 processes, current_index)) {
            break;
        }
        printf("\n다시 입력해주세요.\n");
    }
    
    return create_process(pid, arrival_time, cpu_burst_time, 
                         io_burst_time, io_request_time, priority);
}

int main() {
    printf("CPU 스케줄링 시뮬레이터\n");
    printf("====================\n");
    
    int num_processes;
    Process* processes[MAX_PROCESSES];
    int retry_count = 0;
    
    while (1) {
        printf("생성할 프로세스 수를 입력하세요 (최대 %d): ", MAX_PROCESSES);
        printf("(입력 취소: 'q' 또는 'Q' 입력)\n");
        
        int result = safe_input_int(&num_processes, 1, MAX_PROCESSES, &retry_count);
        if (result == -2) {
            printf("프로그램을 종료합니다.\n");
            return 0;
        }
        if (result <= 0) {
            if (!handle_input_retry(&retry_count, "프로세스 수", 1, MAX_PROCESSES)) return 0;
            continue;
        }
        break;
    }
    
    // 프로세스 정보 입력
    for (int i = 0; i < num_processes; i++) {
        processes[i] = input_process(i + 1, processes, i);
        if (processes[i] == NULL) {
            printf("프로세스 입력이 취소되었습니다.\n");
            // 이미 생성된 프로세스들 메모리 해제
            for (int j = 0; j < i; j++) {
                destroy_process(processes[j]);
            }
            return 0;
        }
    }
    
    // 프로세스 정보 출력
    printf("\n생성된 프로세스 정보:\n");
    for (int i = 0; i < num_processes; i++) {
        print_process_info(processes[i]);
    }
    
    // 메모리 해제
    for (int i = 0; i < num_processes; i++) {
        destroy_process(processes[i]);
    }
    
    return 0;
} 