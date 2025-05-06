#include <stdlib.h>
#include "scheduler.h"

void init_scheduler(void) {}
void add_process(Process* process) { (void)process; }
SchedulingResult* schedule(SchedulingAlgorithm algorithm, int time_quantum) { 
    (void)algorithm;
    (void)time_quantum;
    return NULL; 
}
void print_scheduling_result(SchedulingResult* result) { (void)result; }
void cleanup_scheduler(void) {} 