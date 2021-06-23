#if !defined(_KSYNC_H_)
#define _KSYNC_H_

typedef volatile int spinlock_t;

void acquire_spinlock(spinlock_t *spinlock);
void release_spinlock(spinlock_t *spinlock);

#endif
