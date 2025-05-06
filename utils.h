#ifndef UTILS_H
#define UTILS_H

#include "process.h"

void generate_random_processes(int count);
void handle_io_operation(Process* process);
void evaluate_performance(Process** processes, int count);

#endif // UTILS_H 