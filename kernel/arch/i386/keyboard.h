#pragma once
#ifndef _KEYBOARD_H
#define _KEYBOARD_H 1

#include <idriver.h>

#define KEYBOARD_PORT	0x60

extern "C" void install_keyboard();

class Keyboard : public IDriver
{
public:
	Keyboard(unsigned char id);
	~Keyboard();
	void updateLeds(unsigned char status);
	void install();
};

#define LED_SCROLL_LOCK	1
#define LED_NUM_LOCK	2
#define LED_CAPS_LOCK	4

#define KEY_FLAG_SHIFT	1
#define	KEY_FLAG_ALT	2
#define	KEY_FLAG_ALT_GR	4
#define KEY_FLAG_CTRL	8

#define KEY_A			10
#define KEY_B			11
#define KEY_C			12
#define KEY_D			13
#define KEY_E			14
#define KEY_F			15
#define KEY_G			16
#define KEY_H			17
#define KEY_I			18
#define KEY_J			19
#define KEY_K			20
#define KEY_L			21
#define KEY_M			22
#define KEY_N			23
#define KEY_O			24
#define KEY_P			25
#define KEY_Q			26
#define KEY_R			27
#define KEY_S			28
#define KEY_T			29
#define KEY_U			30
#define KEY_V			31
#define KEY_W			32
#define KEY_X			33
#define KEY_Y			34
#define KEY_Z			35

#define KEY_0			36
#define KEY_1			37
#define KEY_2			38
#define KEY_3			39
#define KEY_4			40
#define KEY_5			41
#define KEY_6			42
#define KEY_7			43
#define KEY_8			44
#define KEY_9			45

#define KEY_SPACE		46
#define KEY_TAB			47
#define	KEY_BACKSPACE	48
#define KEY_RETURN		49

#define KEY_F1			50
#define KEY_F2			51
#define KEY_F3			52
#define KEY_F4			53
#define KEY_F5			54
#define KEY_F6			55
#define KEY_F7			56
#define KEY_F8			57
#define KEY_F9			58
#define KEY_F10			59

#define KEY_COMMA		60
#define KEY_DOT			61

#define KEY_NUM_LOCK	70
#define KEY_CAPS_LOCK	71
#define KEY_SCROLL_LOCK	72
#define	KEY_ARROW_UP	73
#define KEY_ARROW_LEFT	74
#define KEY_ARROW_RIGHT	75
#define KEY_ARROW_DOWN	76
#define KEY_PAGE_UP		77
#define KEY_PAGE_DOWN	78
#define KEY_HOME		79
#define KEY_END			80
#define KEY_INSERT		81
#define KEY_DELETE		82
#define	KEY_SHIFT_RIGHT	83
#define KEY_SHIFT_LEFT	84
#define KEY_ALT			85
#define KEY_ALT_GR		86
#define KEY_CTRL		87

#endif
