#ifndef EVENTS_H
#define EVENTS_H

typedef enum {
    PROCESS_INTERRUPT = 1,
    PROCESS_CREATE = 2,
    PROCESS_FINISH = 3,
    DISK_REQUEST = 4,
    DISK_FINISH = 5,
    MEM_LOAD_REQ = 6,
    MEM_LOAD_FINISH = 7,
    FS_REQUEST = 8,
    FS_FINISH = 9,
    SEMAPHORE_P = 10,
    SEMAPHORE_V = 11,
    PRINT_REQUEST = 14,
    PRINT_FINISH = 15
} event;

int interrupt_control();
int sys_call();

#endif
// Fim do arquivo events.h