#include <gtest/gtest.h>
#include "../include/Utils.h"
#include "../include/TaskTracker.h"
#include <fstream>

static void IsTaskListEqual(const std::unordered_map<uint32_t, TASK>& taskList1, const std::unordered_map<uint32_t, TASK>& taskList2)
{
	ASSERT_EQ(taskList1.size(), taskList2.size());
	
	for (auto itr = taskList1.begin(); itr != taskList1.end(); itr++)
	{
		ASSERT_NE(taskList2.find(itr->first), taskList2.end());
		EXPECT_EQ(itr->second.desc, taskList2.at(itr->first).desc);
		EXPECT_EQ(itr->second.status, taskList2.at(itr->first).status);
		auto tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(itr->second.createdAt - taskList2.at(itr->first).createdAt);
		long long timeDiff = std::abs(tpDiff.count());
		EXPECT_LT(timeDiff, 100);	
		tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(itr->second.createdAt - taskList2.at(itr->first).createdAt);
		timeDiff = std::abs(tpDiff.count());
		EXPECT_LT(timeDiff, 100);
	}
}

class TestWithSampleTasks : public ::testing::Test
{
protected:
	std::unordered_map<uint32_t, TASK> sampleTasks = {
		{0, {"NewTask1", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now()}},
		{1, {"NewTask2", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now()}},
		{2, {"NewTask3", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now()}},
	};

	void TestMarkTasks(TASK_STATUS targetStatus)
	{
		sampleTasks[0].status = TASK_STATUS::IN_PROGRESS;
		sampleTasks[1].status = TASK_STATUS::DONE;
		sampleTasks[2].status = TASK_STATUS::TODO;

		TaskTracker taskTracker({ sampleTasks[0], sampleTasks[1], sampleTasks[2] });
		ASSERT_FALSE(taskTracker.IsJsonLoaded());
		std::unordered_map<uint32_t, TASK> tasksBeforeMark = taskTracker.GetTasks();
		ASSERT_EQ(tasksBeforeMark.size(), sampleTasks.size());

		for (uint32_t id = 0; id < tasksBeforeMark.size(); id++)
		{
			sampleTasks[id].status = targetStatus;
			ASSERT_TRUE(taskTracker.MarkTask(id, targetStatus));
			sampleTasks[id].updatedAt = std::chrono::system_clock::now();
		}

		std::unordered_map<uint32_t, TASK> tasksAfterMark = taskTracker.GetTasks();
		IsTaskListEqual(tasksAfterMark, sampleTasks);
	}

	void TestGetTasksByStatus(TASK_STATUS targetStatus)
	{
		sampleTasks[0].status = TASK_STATUS::DONE;
		sampleTasks[1].status = TASK_STATUS::TODO;
		sampleTasks[2].status = TASK_STATUS::IN_PROGRESS;

		TaskTracker taskTracker({ sampleTasks[0], sampleTasks[1], sampleTasks[2] });
		ASSERT_FALSE(taskTracker.IsJsonLoaded());
		ASSERT_EQ(taskTracker.GetTasks().size(), sampleTasks.size());

		std::unordered_map<uint32_t, TASK> tasksByTargetStatus = taskTracker.GetTasks(&targetStatus);
		ASSERT_EQ(tasksByTargetStatus.size(), 1);
		uint32_t targetTaskId = tasksByTargetStatus.begin()->first;
		ASSERT_NE(sampleTasks.find(targetTaskId), sampleTasks.end());
		IsTaskListEqual(tasksByTargetStatus, {{targetTaskId, sampleTasks[targetTaskId]}});
	}
};

class TestWithJsonFile : public ::testing::Test
{
protected:
	std::string jsonFile = "testsample.json";

	void SetUp() override
	{
		std::remove(jsonFile.c_str());;
	}
	
	void TearDown() override
	{
		std::remove(jsonFile.c_str());
	}

	void CreateEmptyFile()
	{
		TaskTracker taskTracker(jsonFile);
		ASSERT_TRUE(taskTracker.GetTasks().empty());
	}

	void CreateSampleFile(std::unordered_map<uint32_t, TASK>& outSampleTasks)
	{
		TaskTracker taskTracker(jsonFile);
		ASSERT_TRUE(taskTracker.GetTasks().empty());

		std::vector<std::string> samplesTaskDesc = { "ReadTask1", "ReadTask2", "ReadTask3" };
		for (const std::string& taskDesc : samplesTaskDesc)
		{
			ASSERT_TRUE(taskTracker.AddTask(taskDesc));
		}

		outSampleTasks = taskTracker.GetTasks();
		ASSERT_EQ(outSampleTasks.size(), samplesTaskDesc.size());
		
		for (auto itr = outSampleTasks.begin(); itr != outSampleTasks.end(); itr++)
		{
			itr->second.createdAt = StringToTimePoint(TimePointToString(itr->second.createdAt));
			itr->second.updatedAt = StringToTimePoint(TimePointToString(itr->second.updatedAt));
		}
	}
};

TEST_F(TestWithSampleTasks, AddTasks_FromEmptyTaskList)
{
	TaskTracker taskTracker;
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	std::unordered_map<uint32_t, TASK> emptyTasks = taskTracker.GetTasks();
	ASSERT_TRUE(emptyTasks.empty());

	for (uint32_t id = 0; id < TestWithSampleTasks::sampleTasks.size(); id++)
	{
		ASSERT_TRUE(taskTracker.AddTask(sampleTasks[id].desc));
		std::chrono::system_clock::time_point estimateAddTime = std::chrono::system_clock::now();
		sampleTasks[id].createdAt = estimateAddTime;
		sampleTasks[id].updatedAt = estimateAddTime;
	}

	std::unordered_map<uint32_t, TASK> tasksAfterAdd = taskTracker.GetTasks();
	SCOPED_TRACE("AddTasks_FromEmptyTaskList");
	IsTaskListEqual(tasksAfterAdd, sampleTasks);
}

TEST_F(TestWithSampleTasks, UpdateTask_FromEmptyTaskList)
{
	TaskTracker taskTrackerEmpty;
	ASSERT_TRUE(taskTrackerEmpty.GetTasks().empty());
	EXPECT_FALSE(taskTrackerEmpty.UpdateTask(0, "InvalidTaskId"));
	EXPECT_TRUE(taskTrackerEmpty.GetTasks().empty());
}

TEST_F(TestWithSampleTasks, UpdateTask_UsingValidIds)
{
	TaskTracker taskTracker({ sampleTasks[0], sampleTasks[1], sampleTasks[2] });
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	std::unordered_map<uint32_t, TASK> tasksBeforeUpdate = taskTracker.GetTasks();
	ASSERT_EQ(tasksBeforeUpdate.size(), sampleTasks.size());

	std::vector<std::string> taskDescsToUpdate = { "UpdatedTask1", "UpdatedTask2", "UpdatedTask3" };
	for (uint32_t id = 0; id < sampleTasks.size(); id++)
	{
		sampleTasks[id].desc = taskDescsToUpdate[id];
		ASSERT_TRUE(taskTracker.UpdateTask(id, taskDescsToUpdate[id]));
		sampleTasks[id].updatedAt = std::chrono::system_clock::now();
	}

	std::unordered_map<uint32_t, TASK> tasksAfterUpdate = taskTracker.GetTasks();
	SCOPED_TRACE("UpdateTask_UsingValidIds");
	IsTaskListEqual(tasksAfterUpdate, sampleTasks);
}

TEST_F(TestWithSampleTasks, UpdateTask_UsingInvalidId)
{
	TaskTracker taskTracker({ sampleTasks[0], sampleTasks[1], sampleTasks[2] });
	ASSERT_EQ(taskTracker.GetTasks().size(), 3);
	EXPECT_FALSE(taskTracker.UpdateTask(UINT32_MAX, "InvalidTaskId"));
	
	std::unordered_map<uint32_t, TASK> tasksAfterFailUpdate = taskTracker.GetTasks();
	SCOPED_TRACE("UpdateTask_UsingInvalidId");
	IsTaskListEqual(tasksAfterFailUpdate, sampleTasks);
}

TEST_F(TestWithSampleTasks, DeleteTask_OneByOne)
{
	TaskTracker taskTracker({ sampleTasks[0], sampleTasks[1], sampleTasks[2] });
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	uint32_t tasksBeforeDelete = taskTracker.GetTasks().size();
	ASSERT_EQ(tasksBeforeDelete, sampleTasks.size());

	for (uint32_t id = 0; id < tasksBeforeDelete; id++)
	{
		ASSERT_TRUE(taskTracker.DeleteTask(id));
		ASSERT_EQ(sampleTasks.erase(id), 1);

		std::unordered_map<uint32_t, TASK> tasksAfterDelete = taskTracker.GetTasks();
		SCOPED_TRACE("DeleteTask_OneByOne");
		IsTaskListEqual(tasksAfterDelete, sampleTasks);
	}
}

TEST_F(TestWithSampleTasks, MarkTask_AllTodo)
{
	SCOPED_TRACE("MarkTask_AllTodo");
	TestMarkTasks(TASK_STATUS::TODO);
}

TEST_F(TestWithSampleTasks, MarkTask_AllInProgress)
{
	SCOPED_TRACE("MarkTask_AllInProgress");
	TestMarkTasks(TASK_STATUS::IN_PROGRESS);
}

TEST_F(TestWithSampleTasks, MarkTask_AllDone)
{
	SCOPED_TRACE("MarkTask_AllDone");
	TestMarkTasks(TASK_STATUS::DONE);
}

TEST_F(TestWithSampleTasks, GetTasksByStatus_Todo)
{
	SCOPED_TRACE("GetTasksByStatus_Todo");
	TestGetTasksByStatus(TASK_STATUS::TODO);
}

TEST_F(TestWithSampleTasks, GetTasksByStatus_InProgress)
{
	SCOPED_TRACE("GetTasksByStatus_InProgress");
	TestGetTasksByStatus(TASK_STATUS::IN_PROGRESS);
}

TEST_F(TestWithSampleTasks, GetTasksByStatus_Done)
{
	SCOPED_TRACE("GetTasksByStatus_Done");
	TestGetTasksByStatus(TASK_STATUS::DONE);
}

TEST_F(TestWithJsonFile, Construct_FromNonExistJson)
{
	TaskTracker taskTracker(jsonFile);
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	EXPECT_TRUE(taskTracker.GetTasks().empty());
}

TEST_F(TestWithJsonFile, Construct_FromEmptyJson)
{
	CreateEmptyFile();
	TaskTracker taskTracker(jsonFile);
	EXPECT_TRUE(taskTracker.IsJsonLoaded());
	EXPECT_TRUE(taskTracker.GetTasks().empty());
}

TEST_F(TestWithJsonFile, Reconstruct_WriteThenReadFromJson)
{
	std::unordered_map<uint32_t, TASK> sampleTasks;
	CreateSampleFile(sampleTasks);

	TaskTracker taskTrackerRead(jsonFile);
	std::unordered_map<uint32_t, TASK> readTasks = taskTrackerRead.GetTasks();
	IsTaskListEqual(readTasks, sampleTasks);
}