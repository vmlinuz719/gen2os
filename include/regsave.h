#if !defined(_GDT_H_)
#define _GDT_H_
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t RAX,
             RBX,
             RCX,
             RDX,
             RBP,
             RSI,
             RDI,
             RSP,    // not restored automatically
             R8,
             R9,
             R10,
             R11,
             R12,
             R13,
             R14,
             R15,
             RFLAGS, // not restored automatically
             RIP;    // not restored automatically
} EM64RegisterSave;

inline void saveAll(EM64RegisterSave *s) {
    __asm__ __volatile__ ("mov %%rax,%0" : "=r"(s->RAX));
    __asm__ __volatile__ ("mov %%rbx,%0" : "=r"(s->RBX));
    __asm__ __volatile__ ("mov %%rcx,%0" : "=r"(s->RCX));
    __asm__ __volatile__ ("mov %%rdx,%0" : "=r"(s->RDX));
    __asm__ __volatile__ ("mov %%rbp,%0" : "=r"(s->RBP));
    __asm__ __volatile__ ("mov %%rsi,%0" : "=r"(s->RSI));
    __asm__ __volatile__ ("mov %%rdi,%0" : "=r"(s->RDI));
    __asm__ __volatile__ ("mov %%rsp,%0" : "=r"(s->RSP));
    __asm__ __volatile__ ("mov %%r8 ,%0" : "=r"(s->R8));
    __asm__ __volatile__ ("mov %%r9 ,%0" : "=r"(s->R9));
    __asm__ __volatile__ ("mov %%r10,%0" : "=r"(s->R10));
    __asm__ __volatile__ ("mov %%r11,%0" : "=r"(s->R11));
    __asm__ __volatile__ ("mov %%r12,%0" : "=r"(s->R12));
    __asm__ __volatile__ ("mov %%r13,%0" : "=r"(s->R13));
    __asm__ __volatile__ ("mov %%r14,%0" : "=r"(s->R14));
    __asm__ __volatile__ ("mov %%r15,%0" : "=r"(s->R15));
}

inline void restoreAll(EM64RegisterSave *s) {
    __asm__ __volatile__ ("mov %0,%%rax" :: "r"(s->RAX));
    __asm__ __volatile__ ("mov %0,%%rbx" :: "r"(s->RBX));
    __asm__ __volatile__ ("mov %0,%%rcx" :: "r"(s->RCX));
    __asm__ __volatile__ ("mov %0,%%rdx" :: "r"(s->RDX));
    __asm__ __volatile__ ("mov %0,%%rbp" :: "r"(s->RBP));
    __asm__ __volatile__ ("mov %0,%%rsi" :: "r"(s->RSI));
    __asm__ __volatile__ ("mov %0,%%rdi" :: "r"(s->RDI));
    // we can't just __asm__ __volatile__ ("mov %%rsp,%0" :: "r"(s->RSP));
    __asm__ __volatile__ ("mov %0,%%r8" :: "r"(s->R8));
    __asm__ __volatile__ ("mov %0,%%r9" :: "r"(s->R9));
    __asm__ __volatile__ ("mov %0,%%r10" :: "r"(s->R10));
    __asm__ __volatile__ ("mov %0,%%r11" :: "r"(s->R11));
    __asm__ __volatile__ ("mov %0,%%r12" :: "r"(s->R12));
    __asm__ __volatile__ ("mov %0,%%r13" :: "r"(s->R13));
    __asm__ __volatile__ ("mov %0,%%r14" :: "r"(s->R14));
    __asm__ __volatile__ ("mov %0,%%r15" :: "r"(s->R15));
}

#endif
