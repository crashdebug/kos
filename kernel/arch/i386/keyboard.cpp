#include "irq.h"
#include "keyboard.h"
#include "util.h"
#include <kernel.h>
#include <stdio.h>

namespace kos::x86
{
	static Keyboard Keyboard(1);

	Keyboard::Keyboard(unsigned char id)
	{
		printf("Keyboard driver [id 0x%2x]\n", id);
	}

	Keyboard::~Keyboard( )
	{
	}

	void Keyboard::updateLeds(unsigned char status)
	{
		// Wait until input buffer is empty
		while ((inb(0x64) & 2) != 0) ;
		outb(KEYBOARD_PORT, 0xED);

		while ((inb(0x64) & 2) != 0) ;
		outb(KEYBOARD_PORT, status);
	}

	// Basic keyboard layout (US).
	unsigned char keymap[128] =
	{
		0, 0,
		KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
		0, // '-'
		0, // '='
		KEY_BACKSPACE, // '\b'
		0, // '\t'
		KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, 0, 0,
		KEY_RETURN, // '\n'
		0, // Ctrl
		KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, 0, 0, 0,
		0, // Left shift
		0, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, 0, 0, 0,
		0, // Right shift
		0,
		0, // Alt
		KEY_SPACE,
		0, // Caps Lock
		0, // F1
		0, // F2
		0, // F3
		0, // F4
		0, // F5
		0, // F6
		0, // F7
		0, // F8
		0, // F9
		0, // F10
		0, // Num Lock
		0, // Scroll Lock
		0, // Home
		0, // Up Arrow
		0, // Page Up
		0, // Num Minus
		0, // Left Arrow
		0, // Down Arrow
		0, // Right Arrow
		0, // Num Plus
		0, // Page Down
		0, // Insert
		0, // Delete
		0, 0, 0,
		0, // F11
		0, // F12
		0, // All other
	};

	uint8_t MapKeyCode(unsigned char keyCode)
	{
		switch (keyCode)
		{
			case KEY_0:
				return '0';
			case KEY_1:
				return '1';
			case KEY_2:
				return '2';
			case KEY_3:
				return '3';
			case KEY_4:
				return '4';
			case KEY_5:
				return '5';
			case KEY_6:
				return '6';
			case KEY_7:
				return '7';
			case KEY_8:
				return '8';
			case KEY_9:
				return '9';
			case KEY_A:
				return 'a';
			case KEY_B:
				return 'b';
			case KEY_C:
				return 'c';
			case KEY_D:
				return 'd';
			case KEY_E:
				return 'e';
			case KEY_F:
				return 'f';
			case KEY_G:
				return 'g';
			case KEY_H:
				return 'h';
			case KEY_I:
				return 'i';
			case KEY_J:
				return 'j';
			case KEY_K:
				return 'k';
			case KEY_L:
				return 'l';
			case KEY_M:
				return 'm';
			case KEY_N:
				return 'n';
			case KEY_O:
				return 'o';
			case KEY_P:
				return 'p';
			case KEY_Q:
				return 'q';
			case KEY_R:
				return 'r';
			case KEY_S:
				return 's';
			case KEY_T:
				return 't';
			case KEY_U:
				return 'u';
			case KEY_V:
				return 'v';
			case KEY_W:
				return 'w';
			case KEY_X:
				return 'x';
			case KEY_Y:
				return 'y';
			case KEY_Z:
				return 'z';
			case KEY_SPACE:
				return ' ';
			case KEY_BACKSPACE:
				return '\b';
			case KEY_RETURN:
				return '\n';
		}
		return 0;
	}

	// Handles the keyboard interrupt
	void KeyboardHandler(void*)
	{
		// Read from the keyboard's data buffer
		uint8_t scanCode = inb(KEYBOARD_PORT);

		// If the top bit of the byte we read from the keyboard is set,
		// that means that a key has just been released
		if (scanCode & 0x80)
		{
			// You can use this one to see if the user released the shift, alt, or control keys...
		}
		else
		{
			// A key was just pressed.
			// Please note that if you hold a key down, you will get repeated key press interrupts.
			uint8_t mappedChar = MapKeyCode(keymap[scanCode]);
			if (mappedChar != 0)
			{
				keycode_t key;
				key.Key = mappedChar;
				key_pressed(key);
			}
		}
	}

	// Installs the keyboard handler into IRQ1
	void Keyboard::install( )
	{
		install_irq_handler(1, KeyboardHandler);
	}
}

// Tells the kernel about the Keyboard driver. 
// Called from boot.s.
extern "C" void install_keyboard()
{
	install_driver(&kos::x86::Keyboard);
}
