#include <gtest/gtest.h>
#include <command.h>

namespace CommandTests
{
	TEST(CommandTests, CommandParsingTest)
	{
		const char* s = "test 1234 \"foo bar\"";
		kos::ICommand* cmd = kos::CommandParser::Parse(s);
		ASSERT_NE(0, cmd);
		ASSERT_STREQ("test", cmd->cmd());
		ASSERT_EQ(2, cmd->argc());
		ASSERT_STREQ("1234", cmd->arg(0));
		ASSERT_STREQ("foo bar", cmd->arg(1));
		delete cmd;
	}

	class TestCommand : public kos::ICommand
	{
	public:
		TestCommand(const kos::vector<const char*>* args)
		{
			for (int i = 0; i < args->size(); i++)
			{
				this->addParam(args->at(i));
			}
		}
		bool Execute()
		{
			return true;
		}
	};

	static kos::ICommand* _execute_command(const kos::vector<const char*>* args)
	{
		return new TestCommand(args);
	}

	TEST(CommandTests, CommandRegistrationTest)
	{
		kos::CommandParser::Register("test", &_execute_command);
		kos::ICommand* cmd = kos::CommandParser::Parse("test 1234 \"foo bar\"\n");
		ASSERT_NE(0, cmd);
		ASSERT_TRUE(cmd->Execute());
	}
}
