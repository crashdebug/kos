#pragma once
#ifndef _PIC_H

#include <stdint.h>

/* IO base address for master PIC */
#define PIC1			0x20		
/* IO base address for slave PIC */
#define PIC2			0xA0		
#define PIC1_COMMAND	PIC1
#define PIC1_DATA		(PIC1 + 1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA		(PIC2 + 1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */

void send_pic_eoi(unsigned char irq);

unsigned char pic_irq_mask(uint16_t pic);

void pic_set_irq_mask(unsigned char irq);
void pic_clear_mask(unsigned char irq);

// Arguments:
//	master_offset - vector offset for master PIC vectors on the master become offset1..offset1+7
//	slave_offset - same for slave PIC: offset2..offset2+7
void pic_remap(int master_offset, int slave_offset);

#endif
