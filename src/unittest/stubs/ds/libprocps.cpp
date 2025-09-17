extern "C" {
#include <proc/readproc.h>
}

static int call_count = 0;

PROCTAB* openproc(int flags, ...) {
    // Your stub implementation here
    return nullptr;
}

void closeproc(PROCTAB* pt) {
    // Your stub implementation here
}

proc_t* readproc(PROCTAB* pt, proc_t* buffer) {
    // Your stub implementation here
    return nullptr;
}
