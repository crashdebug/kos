#pragma once
#ifndef _IRQ_H
#define _IRQ_H 1

#include "registers.h"

#ifdef __cplusplus
extern "C" {
#endif

void disableInterrupts();
void enableInterrupts();
void install_irq();
void install_irq_handler(int irq, void(*handler)(struct Registers*));
void uninstall_irq_handler(int irq);
void wait_for_irq(int irq);

#ifdef __cplusplus
}
#endif

struct IRQCallback
{
	void (*handler)(struct Registers*);
	unsigned char count;
};

#endif
