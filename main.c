#include <stdio.h>
#include "process.h"
#include "scheduler.h"

int main() {
    printf("CPU 스케줄링 시뮬레이터\n");
    printf("====================\n");
    
    // 테스트용 프로세스 생성
    Process* p1 = create_process(1, 0, 5, 2, 3, 1);
    Process* p2 = create_process(2, 1, 3, 1, 2, 2);
    
    // 프로세스 정보 출력
    printf("\n생성된 프로세스 정보:\n");
    print_process_info(p1);
    print_process_info(p2);
    
    // 메모리 해제
    destroy_process(p1);
    destroy_process(p2);
    
    return 0;
} 