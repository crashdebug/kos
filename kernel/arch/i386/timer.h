#pragma once
#ifndef _TIMER_H
#define _TIMER_H 1

#include <idriver.h>

#define TIMER_E	( 3000.0 / 3579545 )

extern "C" void install_timer();

class Timer : public IDriver
{
	public:
		void install();
};

static Timer Timer;

#endif
