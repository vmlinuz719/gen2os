// VERY basic implementation of a spinlock, thanks to osdev wiki

#include <ksync.h>

void acquire_spinlock(spinlock_t *spinlock) {
    while (!__sync_bool_compare_and_swap(spinlock, 0, 1)) {
        asm("pause");
    }
}

void release_spinlock(spinlock_t *spinlock) {
    *spinlock = 0;
}
