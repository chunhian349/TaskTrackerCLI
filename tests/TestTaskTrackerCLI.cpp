#include <gtest/gtest.h>

static std::string ExecuteCLI(const std::string& command)
{
	const int bufSize = 128;
	char buffer[128] = {};
	FILE* pPipe = _popen(command.c_str(), "r");
	if (!pPipe) { return ""; }

	std::string CLIOutput;
	while (fgets(buffer, bufSize, pPipe))
	{
		CLIOutput += buffer;
	}

	return CLIOutput;
}

class TestWithEmptyJson : public ::testing::Test
{
protected:
	std::string jsonFile = "tasks.json";
	std::string exeFile = TASKTRACKERCLI_EXE_PATH;

	void SetUp() override
	{
		std::remove(jsonFile.c_str());
	}

	void TearDown() override
	{
		std::remove(jsonFile.c_str());
	}

	void CreateSampleFile()
	{
		std::string command = exeFile + " " + "add \"Sample Task 1\"";
		std::string CLIOutput = ExecuteCLI(command);
		ASSERT_NE(CLIOutput.find("Task added successfully"), std::string::npos);
	}
};

TEST_F(TestWithEmptyJson, EmptyCommand)
{
	std::string CLIOutput = ExecuteCLI(exeFile);
	EXPECT_NE(CLIOutput.find("Unable to read file"), std::string::npos);
	EXPECT_NE(CLIOutput.find("No command entered"), std::string::npos);
}

TEST_F(TestWithEmptyJson, InvalidCommand)
{
	std::string command = exeFile + " " + "random";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Invalid command entered"), std::string::npos);
}

TEST_F(TestWithEmptyJson, AddTask)
{
	std::string command = exeFile + " " + "add \"New Task Desc\"";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Task added successfully"), std::string::npos);
}

TEST_F(TestWithEmptyJson, AddTask_MissingArguments)
{
	std::string command = exeFile + " " + "add";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Insufficient add task argument"), std::string::npos);
}

TEST_F(TestWithEmptyJson, UpdateTask)
{
	CreateSampleFile();
	std::string command = exeFile + " " + "update 0 \"Update Task Desc\"";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Task updated successfully"), std::string::npos);
}

TEST_F(TestWithEmptyJson, UpdateTask_EmptyList)
{
	std::string command = exeFile + " " + "update 0 \"Update Task Desc\"";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Update task failed"), std::string::npos);
}

TEST_F(TestWithEmptyJson, UpdateTask_MissingArguments)
{
	std::string command = exeFile + " " + "update";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Insufficient update task argument"), std::string::npos);
}

TEST_F(TestWithEmptyJson, DeleteTask)
{
	CreateSampleFile();
	std::string commmand = exeFile + " " + "delete 0";
	std::string CLIOutput = ExecuteCLI(commmand);
	EXPECT_NE(CLIOutput.find("Task deleted successfully"), std::string::npos);
}

TEST_F(TestWithEmptyJson, DeleteTask_EmptyList)
{
	std::string commmand = exeFile + " " + "delete 0";
	std::string CLIOutput = ExecuteCLI(commmand);
	EXPECT_NE(CLIOutput.find("Delete task failed"), std::string::npos);
}

TEST_F(TestWithEmptyJson, DeleteTask_MissingArguments)
{
	std::string command = exeFile + " " + "delete";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Insufficient delete task argument"), std::string::npos);
}

TEST_F(TestWithEmptyJson, MarkTask)
{
	CreateSampleFile();
	std::string command = exeFile + " " + "mark-in-progress 0";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Mark task successfully"), std::string::npos);
}

TEST_F(TestWithEmptyJson, MarkTask_EmptyList)
{
	std::string command = exeFile + " " + "mark-in-progress 0";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Mark task failed"), std::string::npos);
}

TEST_F(TestWithEmptyJson, MarkTask_MissingArguments)
{
	std::string command = exeFile + " " + "mark-in-progress";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Insufficient mark task argument"), std::string::npos);
}

TEST_F(TestWithEmptyJson, ListTask)
{
	CreateSampleFile();
	std::string command = exeFile + " " + "list";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("=Task list="), std::string::npos);
}

TEST_F(TestWithEmptyJson, ListTask_EmptyList)
{
	std::string command = exeFile + " " + "list";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("Task list is empty"), std::string::npos);
}

TEST_F(TestWithEmptyJson, ListTaskInvalidStatus)
{
	std::string command = exeFile + " " + "list random";
	std::string CLIOutput = ExecuteCLI(command);
	EXPECT_NE(CLIOutput.find("List task failed"), std::string::npos);
}