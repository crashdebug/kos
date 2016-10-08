#pragma once
#ifndef _KERNEL_H
#define _KERNEL_H 1

#include <idriver.h>
#include <time.h>
#include <stdint.h>

#define KOS_VERSION		0,0,1
#define KOS_VERSION_STR	"0.0.1"

struct KeyCode
{
	bool Shift;
	bool Alt;
	bool AltGr;
	bool Ctrl;
	uint8_t Key;
};

typedef struct KeyCode keycode_t;

void install_driver(kos::IDriver* driver);

#ifdef __cplusplus
extern "C" {
#endif

void set_ticks(time_t ticks);
time_t ticks();

void key_pressed(struct KeyCode key);
keycode_t read_key();

#ifdef __cplusplus
}
#endif

#endif
