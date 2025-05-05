#ifndef PARSER_H
#define PARSER_H
#define MAX_INSTR 100
#define MAX_NAME 32
#define MAX_SEM 10

#include "../include/bcp.h"
#include "../include/cpu.h"
#include "../include/memory.h"
#include "../include/scheduler.h"
#include "../include/events.h"

BCP *load_program(const char *file_path, int next_id);

#endif
