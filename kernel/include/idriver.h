#pragma once
#ifndef _IDRIVER_H
#define _IDRIVER_H 1

namespace kos
{
	class IDriver
	{
	public:
		virtual void install() = 0;
	};
}

#endif
