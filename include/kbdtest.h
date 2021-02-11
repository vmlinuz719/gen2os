#if !defined(_KBDTEST_H_)
#define _KBDTEST_H_

#include <stdint.h>
#include <stddef.h>

typedef struct xsf {
	uint64_t RIP;
	uint64_t CS;
	uint64_t RFLAGS;
	uint64_t RSP;
	uint64_t SS;
} ExceptionStackFrame;

void kbdTest(ExceptionStackFrame *frame);
void KIWF(ExceptionStackFrame *frame, unsigned long int errorCode);
void illegal(ExceptionStackFrame *frame);
void generalProtection(ExceptionStackFrame *frame, unsigned long int errorCode);

#endif
