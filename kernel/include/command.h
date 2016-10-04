#pragma once
#ifndef _COMMAND_H
#define _COMMAND_H 1

#include <vector.h>

class ICommand
{
public:
	virtual ~ICommand();
	virtual bool Execute() = 0;
	unsigned char argc();
	const char* arg(unsigned char index);
	const vector<const char*>* args();
	void addParam(const char* param);
	const char* cmd();
	void setCmd(const char* cmd);
protected:
	vector<const char*> _args;
	const char* _cmd = 0;
};

class CommandParser
{
private:
	class Command : public ICommand
	{
	public:
		bool Execute();
	};

	static void append(ICommand* cmd, unsigned char* state, const char* s, unsigned int len);
public:
	static void Register(const char* cmd, ICommand* (*ctor)(const vector<const char*>*));
	static ICommand* Parse(const char* input);
};

#endif
