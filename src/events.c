#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/events.h"

int interrupt_control() {  
}

int sys_call(event e) {
    switch (e){
    case PROCESS_INTERRUPT:
        /* code */
        break;
    case PROCESS_CREATE:
        /* code */
        break;
    case PROCESS_FINISH:
        /* code */
        break;
    case DISK_REQUEST:
        /* code */
        break;
    case DISK_FINISH:
        /* code */
        break;
    case MEM_LOAD_REQ:
        /* code */
        break;
    case MEM_LOAD_FINISH:
        /* code */
        break;
    case FS_REQUEST:
        /* code */
        break;
    case FS_FINISH:
        /* code */
        break;
    case SEMAPHORE_P:
        /* code */
        break;
    case SEMAPHORE_V:
        /* code */
        break;
    case PRINT_REQUEST:
        /* code */
        break;
    case PRINT_FINISH:
        /* code */
        break;
    default:
        printf(stderr, "Unknown event: %d\n", e);
        return -1;
        break;
    }
    return 0;
}