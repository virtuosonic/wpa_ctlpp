#include "includes.h"

#include <time.h>

#include "utils/os.h"

int os_get_reltime(struct os_reltime *time)
{
    struct timespec current_time;

    if (clock_gettime(CLOCK_MONOTONIC, &current_time) != 0)
        return -1;

    time->sec = current_time.tv_sec;
    time->usec = current_time.tv_nsec / 1000;
    return 0;
}

void os_sleep(os_time_t sec, os_time_t usec)
{
    struct timespec requested = {
        .tv_sec = sec + usec / 1000000,
        .tv_nsec = (usec % 1000000) * 1000
    };

    while (nanosleep(&requested, &requested) != 0 && errno == EINTR)
        ;
}