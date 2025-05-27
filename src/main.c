#include "scheduler.h"
#include "utils.h"
#include <stdio.h>

int main() {
    Scheduler* scheduler = Config();

    for (int i = 0; i < scheduler->process_cnt; i++) {
        print_process(scheduler->process_arr[i]);
    }

    Simulate(scheduler);
    Evaluation(scheduler);

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