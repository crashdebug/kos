#include "command.h"
#include <string.h>
#include <stdio.h>

namespace kos
{
	struct CommandBucket
	{
		const char* cmd;
		ICommand* (*ctor)(const vector<const char*>*);
	};

	vector<CommandBucket> s_ctors;

	void CommandParser::Register(const char* cmd, ICommand* (*ctor)(const vector<const char*>*))
	{
		CommandBucket bucket;
		bucket.cmd = cmd;
		bucket.ctor = ctor;
		s_ctors.push_back(bucket);
	}

	void CommandParser::append(ICommand* cmd, unsigned char* state, const char* s, unsigned int len)
	{
		char* temp;
		switch (*state)
		{
			case 0:
				temp = new char[len];
				memcpy(temp, s, len);
				temp[len] = 0;
				cmd->setCmd(temp);
				(*state)++;
				break;
			case 1:
				temp = new char[len];
				memcpy(temp, s, len);
				temp[len] = 0;
				cmd->addParam(temp);
				break;
		}
	}

	ICommand* CommandParser::Parse(const char* input)
	{
		if (input == 0)
		{
			return 0;
		}

		const char* pos = input;
		const char* start = input;
		char quote = 0;
		unsigned char state = 0;
		ICommand* cmd = new CommandParser::Command;
		do
		{
			if (*pos == '\'' || *pos == '"')
			{
				if (quote == 0)
				{
					quote = *pos;
					start++;
				}
				else if (quote == *pos)
				{
					quote = 0;
					append(cmd, &state, start, pos - start);
					start = pos + 1;
				}
			}
			else if ((quote == 0 && (*pos == ' ' || *pos == '\t')) || *pos == 0)
			{
				if (pos - start > 0)
				{
					append(cmd, &state, start, pos - start);
				}
				start = pos + 1;
			}
			if (*pos == 0)
			{
				break;
			}
			pos++;
		} while (true);

		for (unsigned int i = 0; i < s_ctors.size(); i++)
		{
			if (strcmp(s_ctors.at(i).cmd, cmd->cmd()) == 0)
			{
				ICommand* temp = s_ctors.at(i).ctor(cmd->args());
				delete cmd;
				cmd = temp;
				break;
			}
		}

		return cmd;
	}

	bool CommandParser::Command::Execute()
	{
		printf("Unknown command '%s'", this->_cmd);
		return false;
	}

	ICommand::~ICommand()
	{
		if (this->_cmd != 0)
		{
			delete this->_cmd;
		}
		for (unsigned char i = 0; i < this->_args.size(); i++)
		{
			delete this->_args.at(i);
		}
	}

	void ICommand::addParam(const char* param)
	{
		this->_args.push_back(param);
	}

	void ICommand::setCmd(const char* cmd)
	{
		this->_cmd = cmd;
	}

	const char* ICommand::cmd()
	{
		return this->_cmd;
	}

	unsigned char ICommand::argc()
	{
		return this->_args.size();
	}

	const char* ICommand::arg(unsigned char index)
	{
		return this->_args.at(index);
	}

	const vector<const char*>* ICommand::args()
	{
		return &this->_args;
	}
}
