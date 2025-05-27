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
    }
    printf("%-6d\n", scheduler->gantt_chart[scheduler->gantt_chart_cnt - 1].end_time);

    return 0;
}