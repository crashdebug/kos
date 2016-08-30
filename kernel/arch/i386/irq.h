#pragma once
#ifndef _IRQ_H
#define _IRQ_H 1

#include "registers.h"

void disableInterrupts();
void enableInterrupts();
void install_irq();
void installHandler(int irq, void(*handler)(struct Registers*));
void uninstallHandler(int irq);
void waitForIRQ(int irq);

struct IRQCallback
{
	void (*handler)(struct Registers*);
	unsigned char count;
};

#endif
