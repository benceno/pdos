#include "realtime_queue.h"

struct realtime_queue createRealTimeQueue(){
    struct listproc list;
    list.head = 0;
    list.tail = 0;
    list.size = 0;

    struct realtime_queue queue;
    queue.list = list;

    return queue;
}

