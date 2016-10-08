#pragma once
#ifndef _VECTOR_H
#define _VECTOR_H 1

#ifdef TEST
#include "../../libc/include/string.h"
#else
#include <string.h>
#endif

namespace kos
{
	template <typename T>
	class vector
	{
	public:
		vector()
		{
		}
		~vector()
		{
			if (this->_items != 0)
			{
				delete this->_items;
			}
		}
		void push_back(const T& value)
		{
			this->ensure_capacity(this->size() + 1);
			this->_items[this->_pos++] = value;
		}
		T at(unsigned int pos) const
		{
			return this->_items[pos];
		}
		unsigned int size() const
		{
			return this->_pos;
		}
	private:
		T* _items = 0;
		unsigned int _pos = 0;
		unsigned int _capacity = 0;

		void ensure_capacity(unsigned int count)
		{
			if (this->_capacity - this->_pos > count)
			{
				return;
			}
			if (this->_items == 0)
			{
				this->_items = new T[1];
				this->_capacity = 1;
			}
			else
			{
				T* temp = new T[this->_capacity * 2];
				memcpy(temp, this->_items, sizeof(T) * this->_pos);
				delete[] this->_items;
				this->_items = temp;
				this->_capacity *= 2;
			}
		}
	};
}

#endif
