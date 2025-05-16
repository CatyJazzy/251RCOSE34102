#include <stdio.h>
#include <assert.h>
#include "scheduler.h"
#include "process.h"

// 큐 테스트 함수
void test_queue_operations() {
    printf("\n=== 큐 테스트 시작 ===\n");
    
    Queue queue;
    init_queue(&queue);
    
    // 1. 빈 큐 테스트
    assert(is_queue_empty(&queue) == true);
    assert(queue.size == 0);
    printf("빈 큐 테스트 통과\n");
    
    // 2. 프로세스 추가 테스트
    Process* p1 = create_process(1, 0, 5, 2, 2, 1);
    Process* p2 = create_process(2, 1, 3, 1, 1, 2);
    
    enqueue(&queue, p1);
    assert(queue.size == 1);
    assert(queue.front->process == p1);
    assert(queue.rear->process == p1);
    
    enqueue(&queue, p2);
    assert(queue.size == 2);
    assert(queue.front->process == p1);
    assert(queue.rear->process == p2);
    printf("프로세스 추가 테스트 통과\n");
    
    // 3. 프로세스 제거 테스트
    Process* dequeued = dequeue(&queue);
    assert(dequeued == p1);
    assert(queue.size == 1);
    assert(queue.front->process == p2);
    assert(queue.rear->process == p2);
    printf("프로세스 제거 테스트 통과\n");
    
    // 4. 큐 비우기 테스트
    clear_queue(&queue);
    assert(is_queue_empty(&queue) == true);
    assert(queue.size == 0);
    printf("큐 비우기 테스트 통과\n");
    
    // 메모리 정리
    destroy_process(p1);
    destroy_process(p2);
    
    printf("=== 큐 테스트 완료 ===\n");
}

// 스케줄러 테스트 함수
void test_scheduler_operations() {
    printf("\n=== 스케줄러 테스트 시작 ===\n");
    
    // 1. 스케줄러 초기화 테스트
    init_scheduler();
    assert(is_queue_empty(&scheduler.ready_queue) == true);
    assert(is_queue_empty(&scheduler.waiting_queue) == true);
    assert(scheduler.running_process == NULL);
    assert(scheduler.time_quantum == 0);
    assert(scheduler.current_algorithm == FCFS);
    printf("스케줄러 초기화 테스트 통과\n");
    
    // 2. 스케줄러 설정 테스트
    config_scheduler(ROUND_ROBIN, 2);
    assert(scheduler.current_algorithm == ROUND_ROBIN);
    assert(scheduler.time_quantum == 2);
    assert(is_queue_empty(&scheduler.ready_queue) == true);
    assert(is_queue_empty(&scheduler.waiting_queue) == true);
    printf("스케줄러 설정 테스트 통과\n");
    
    // 3. 프로세스 추가 테스트
    Process* p1 = create_process(1, 0, 5, 2, 2, 1);
    Process* p2 = create_process(2, 1, 3, 1, 1, 2);
    
    add_process(p1);
    assert(scheduler.ready_queue.size == 1);
    assert(scheduler.ready_queue.front->process == p1);
    assert(p1->state == READY);
    
    add_process(p2);
    assert(scheduler.ready_queue.size == 2);
    assert(scheduler.ready_queue.rear->process == p2);
    assert(p2->state == READY);
    printf("프로세스 추가 테스트 통과\n");
    
    // 4. 스케줄러 정리 테스트
    cleanup_scheduler();
    assert(is_queue_empty(&scheduler.ready_queue) == true);
    assert(is_queue_empty(&scheduler.waiting_queue) == true);
    assert(scheduler.running_process == NULL);
    printf("스케줄러 정리 테스트 통과\n");
    
    // 메모리 정리
    destroy_process(p1);
    destroy_process(p2);
    
    printf("=== 스케줄러 테스트 완료 ===\n");
}

int main() {
    printf("스케줄러 테스트 프로그램 시작\n");
    
    test_queue_operations();
    test_scheduler_operations();
    
    printf("\n모든 테스트가 성공적으로 완료되었습니다!\n");
    return 0;
} 