#pragma once
#ifndef _TIMER_H
#define _TIMER_H 1

#include <idriver.h>

extern "C" void install_timer();

class Timer : public IDriver
{
	public:
		void install();
};

static Timer Timer;

#endif
