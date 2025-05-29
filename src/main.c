#include "scheduler.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    srand(time(NULL));

    Scheduler* scheduler = Config();

    for (int i = 0; i < scheduler->process_cnt; i++) {
        print_process(scheduler->process_arr[i]);
    }

    Simulate(scheduler);
    Evaluation(scheduler);

    // 간트차트 시간순 정렬
    for (int i=0; i<scheduler->gantt_chart_cnt; i++) {
        for (int j=i+1; j<scheduler->gantt_chart_cnt; j++) {
            if (scheduler->gantt_chart[i].start_time > scheduler->gantt_chart[j].start_time) {
                GanttChart temp = scheduler ->gantt_chart[i];
                scheduler->gantt_chart[i] = scheduler->gantt_chart[j];
                scheduler->gantt_chart[j] = temp;
            }
        }
    } 

    /* 디버깅 용도 */
    // main.c에서 간트 차트 출력 전
    // printf("DEBUG: 간트차트 데이터 확인\n");
    // for (int i = 0; i < scheduler->gantt_chart_cnt; i++) {
    // printf("인덱스 %d: %s (%d-%d)\n", 
    //         i, scheduler->gantt_chart[i].process_name,
    //         scheduler->gantt_chart[i].start_time,
    //         scheduler->gantt_chart[i].end_time);
    // }


    printf("\n--------- 시뮬레이션 프로세스 정보 ---------\n");
    for (int i = 0; i < scheduler->process_cnt; i++) {
        print_process(scheduler->process_arr[i]);
    }
    printf("\n\n");

    // 간트차트 출력 부분
    for (int i=0; i<scheduler->gantt_chart_cnt; i++) {
        printf("-------");
    }
    printf("-\n");

    for (int i=0; i<scheduler->gantt_chart_cnt; i++) {
        printf("| %-5s", scheduler->gantt_chart[i].process_name);
    }
    printf("|\n");

    for (int i=0; i< scheduler->gantt_chart_cnt; i++) {
        printf("-------");
    }
    printf("-\n");

    for (int i=0; i<scheduler->gantt_chart_cnt; i++) {
        printf("%-7d", scheduler->gantt_chart[i].start_time);

        if(i== scheduler->gantt_chart_cnt - 1) {
            printf("%-6d\n", scheduler->gantt_chart[i].end_time);
        }
    }


    for (int i=0; i<scheduler->process_cnt; i++) {
        if (scheduler->process_arr[i] != NULL) {
            destroy_process(scheduler->process_arr[i]);
        }
    }
    free(scheduler->process_arr);
    free(scheduler->ready_queue);
    free(scheduler->waiting_queue);
    free(scheduler);

    return 0;
}