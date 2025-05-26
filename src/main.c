#include "scheduler.h"

int main() {
    Scheduler* scheduler = Config();

    for (int i = 0; i < scheduler->process_cnt; i++) {
        print_process(scheduler->process_arr[i]);
    }

    Simulate(scheduler);
    Evaluation(scheduler);

    return 0;
}