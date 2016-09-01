#pragma once
#ifndef _TIMER_H
#define _TIMER_H 1

#define TIMER_E	( 3000.0 / 3579545 )

class Timer
{
	public:
		void Install();
};

static Timer Timer;

#endif
