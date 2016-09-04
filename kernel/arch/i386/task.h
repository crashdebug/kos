#pragma once
#ifndef _TASK_H
#define _TASK_H 1

#include <stdint.h>

// Task control block (structure to be used in a 2-way linked list)
typedef struct __attribute__((packed)) task_t
{
	// CPU registers (same format as in the interrupted task stack)
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp_discarded;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp;
	uint32_t ss;
	uint32_t es;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	// b0=paused, b1=exception waiting to be handled by the monitor, b2=error, b3=successfully finished
	uint32_t taskflags;
	// Number of ticks the task has been executing since it started its turn
	uint32_t ticks;
	// Maximum number of ticks allowed per turn, when ticks == maxticks, set ticks to 0 and switch to next task
	uint32_t maxticks; 
	// (for exceptions) exception number which interrupted the task
	uint32_t excep_num;
	// (for exceptions) exception error code
	uint32_t excep_code;
	// Pointer to previous task control block
	struct task_t *prev;
	// Pointer to next task control block
	struct task_t *next;
} task_t;

#endif
