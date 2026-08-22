#include <gtest/gtest.h>
#include "../include/Utils.h"
#include "../include/TaskTracker.h"
#include <fstream>

TEST(AddTask, AddTasks)
{
	TaskTracker taskTracker;
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	std::unordered_map<uint32_t, TASK> emptyTasks = taskTracker.GetTasks();
	ASSERT_TRUE(emptyTasks.empty());

	std::vector<std::string> tasksDescToAdd = { "NewTask1", "NewTask2", "NewTask3" };
	std::vector<std::chrono::system_clock::time_point> tasksAddTime;
	for (const std::string& taskDesc : tasksDescToAdd)
	{
		ASSERT_TRUE(taskTracker.AddTask(taskDesc));
		tasksAddTime.push_back(std::chrono::system_clock::now());
	}

	std::unordered_map<uint32_t, TASK> tasksAfterAdd = taskTracker.GetTasks();
	ASSERT_EQ(tasksAfterAdd.size(), tasksDescToAdd.size());

	for (uint32_t id=0; id< tasksAfterAdd.size(); id++)
	{
		ASSERT_NE(tasksAfterAdd.find(id), tasksAfterAdd.end());
		EXPECT_EQ(tasksAfterAdd[id].desc, tasksDescToAdd[id]);
		EXPECT_EQ(tasksAfterAdd[id].status, TASK_STATUS::TODO);
		auto tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(tasksAddTime[id] - tasksAfterAdd[id].createdAt);
		long long timeDiff = std::abs(tpDiff.count());
		EXPECT_LT(timeDiff, 100);
		tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(tasksAddTime[id] - tasksAfterAdd[id].updatedAt);
		timeDiff = std::abs(tpDiff.count());
		EXPECT_LT(timeDiff, 100);
	}
}

TEST(UpdateTask, FromEmptyTask)
{
	TaskTracker taskTrackerEmpty;
	ASSERT_TRUE(taskTrackerEmpty.GetTasks().empty());
	EXPECT_FALSE(taskTrackerEmpty.UpdateTask(0, "InvalidTaskId"));
	EXPECT_TRUE(taskTrackerEmpty.GetTasks().empty());
}

TEST(UpdateTask, UsingValidIds)
{
	std::vector<TASK> sampleTasks = { 
		{ "NewTask1", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask2", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask3", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
	};
	TaskTracker taskTracker(sampleTasks);
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	ASSERT_EQ(taskTracker.GetTasks().size(), sampleTasks.size());

	std::vector<std::string> tasksDescToUpdate = { "UpdatedTask1", "UpdatedTask2", "UpdatedTask3" };
	std::vector<std::chrono::system_clock::time_point> tasksUpdateTime;
	for (uint32_t id = 0; id < sampleTasks.size(); id++)
	{
		ASSERT_TRUE(taskTracker.UpdateTask(id, tasksDescToUpdate[id]));
		tasksUpdateTime.push_back(std::chrono::system_clock::now());
	}

	std::unordered_map<uint32_t, TASK> tasksAfterUpdate = taskTracker.GetTasks();
	ASSERT_EQ(tasksAfterUpdate.size(), sampleTasks.size());
	for (uint32_t id = 0; id < tasksAfterUpdate.size(); id++)
	{
		ASSERT_NE(tasksAfterUpdate.find(id), tasksAfterUpdate.end());
		EXPECT_EQ(tasksAfterUpdate[id].desc, tasksDescToUpdate[id]);
		EXPECT_EQ(tasksAfterUpdate[id].status, sampleTasks[id].status);
		EXPECT_EQ(tasksAfterUpdate[id].createdAt, sampleTasks[id].createdAt);
		auto tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(tasksUpdateTime[id] - tasksAfterUpdate[id].updatedAt);
		long long timeDiff = std::abs(tpDiff.count());
		EXPECT_LT(timeDiff, 100);
	}
}

TEST(UpdateTask, UsingInvalidId)
{
	std::vector<TASK> sampleTasks = {
		{ "NewTask1", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask2", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask3", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
	};
	TaskTracker taskTracker(sampleTasks);
	ASSERT_EQ(taskTracker.GetTasks().size(), 3);
	EXPECT_FALSE(taskTracker.UpdateTask(UINT32_MAX, "InvalidTaskId"));
	
	std::unordered_map<uint32_t, TASK> tasks = taskTracker.GetTasks();
	ASSERT_EQ(tasks.size(), 3);

	for (uint32_t id = 0; id < tasks.size(); id++)
	{
		ASSERT_NE(tasks.find(id), tasks.end());
		EXPECT_EQ(tasks[id].desc, sampleTasks[id].desc);
		EXPECT_EQ(tasks[id].status, sampleTasks[id].status);
		EXPECT_EQ(tasks[id].createdAt, sampleTasks[id].createdAt);
		EXPECT_EQ(tasks[id].updatedAt, sampleTasks[id].updatedAt);
	}
}

TEST(DeleteTask, DeleteOneByOne)
{
	std::vector<TASK> sampleTasks = {
		{ "NewTask1", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask2", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask3", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
	};
	TaskTracker taskTracker(sampleTasks);
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	std::unordered_map<uint32_t, TASK> tasksBeforeDelete = taskTracker.GetTasks();
	ASSERT_EQ(tasksBeforeDelete.size(), sampleTasks.size());

	ASSERT_TRUE(taskTracker.DeleteTask(0));

	std::unordered_map<uint32_t, TASK> tasksAfterDelete = taskTracker.GetTasks();
	ASSERT_EQ(tasksAfterDelete.size(), 2);
	for (uint32_t id = 1; id < tasksBeforeDelete.size(); id++)
	{
		ASSERT_NE(tasksAfterDelete.find(id), tasksAfterDelete.end());
		EXPECT_EQ(tasksAfterDelete[id].desc, tasksBeforeDelete[id].desc);
		EXPECT_EQ(tasksAfterDelete[id].status, tasksBeforeDelete[id].status);
		EXPECT_EQ(tasksAfterDelete[id].createdAt, tasksBeforeDelete[id].createdAt);
		EXPECT_EQ(tasksAfterDelete[id].updatedAt, tasksBeforeDelete[id].updatedAt);
	}

	ASSERT_TRUE(taskTracker.DeleteTask(1));

	tasksAfterDelete = taskTracker.GetTasks();
	ASSERT_EQ(tasksAfterDelete.size(), 1);
	for (uint32_t id = 2; id < tasksBeforeDelete.size(); id++)
	{
		ASSERT_NE(tasksAfterDelete.find(id), tasksAfterDelete.end());
		EXPECT_EQ(tasksAfterDelete[id].desc, tasksBeforeDelete[id].desc);
		EXPECT_EQ(tasksAfterDelete[id].status, tasksBeforeDelete[id].status);
		EXPECT_EQ(tasksAfterDelete[id].createdAt, tasksBeforeDelete[id].createdAt);
		EXPECT_EQ(tasksAfterDelete[id].updatedAt, tasksBeforeDelete[id].updatedAt);
	}

	ASSERT_TRUE(taskTracker.DeleteTask(2));

	tasksAfterDelete = taskTracker.GetTasks();
	EXPECT_TRUE(tasksAfterDelete.empty());
}

TEST(MarkTest, MarkAllTodo)
{
	std::vector<TASK> sampleTasks = {
		{ "NewTask1", TASK_STATUS::IN_PROGRESS, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask2", TASK_STATUS::DONE, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask3", TASK_STATUS::IN_PROGRESS, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
	};
	TaskTracker taskTracker(sampleTasks);
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	std::unordered_map<uint32_t, TASK> tasksBeforeMark = taskTracker.GetTasks();
	ASSERT_EQ(tasksBeforeMark.size(), sampleTasks.size());

	std::vector<std::chrono::system_clock::time_point> taskMarkTimes;
	for (uint32_t id = 0; id < tasksBeforeMark.size(); id++)
	{
		ASSERT_TRUE(taskTracker.MarkTask(id, TASK_STATUS::TODO));
		taskMarkTimes.push_back(std::chrono::system_clock::now());
	}

	std::unordered_map<uint32_t, TASK> tasksAfterMark = taskTracker.GetTasks();
	ASSERT_EQ(tasksAfterMark.size(), tasksBeforeMark.size());
	for (uint32_t id = 0; id < tasksAfterMark.size(); id++)
	{
		ASSERT_NE(tasksAfterMark.find(id), tasksAfterMark.end());
		EXPECT_EQ(tasksAfterMark[id].desc, tasksBeforeMark[id].desc);
		EXPECT_EQ(tasksAfterMark[id].status, TASK_STATUS::TODO);
		EXPECT_EQ(tasksAfterMark[id].createdAt, tasksBeforeMark[id].createdAt);
		auto tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(taskMarkTimes[id] - tasksAfterMark[id].updatedAt);
		long long timeDiff = std::abs(tpDiff.count());
		EXPECT_LT(timeDiff, 100);
	}
}

TEST(MarkTask, MarkAllInProgress)
{
	std::vector<TASK> sampleTasks = {
		{ "NewTask1", TASK_STATUS::DONE, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask2", TASK_STATUS::DONE, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask3", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
	};
	TaskTracker taskTracker(sampleTasks);
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	std::unordered_map<uint32_t, TASK> tasksBeforeMark = taskTracker.GetTasks();
	ASSERT_EQ(tasksBeforeMark.size(), sampleTasks.size());

	std::vector<std::chrono::system_clock::time_point> taskMarkTimes;
	for (uint32_t id = 0; id < tasksBeforeMark.size(); id++)
	{
		ASSERT_TRUE(taskTracker.MarkTask(id, TASK_STATUS::IN_PROGRESS));
		taskMarkTimes.push_back(std::chrono::system_clock::now());
	}

	std::unordered_map<uint32_t, TASK> tasksAfterMark = taskTracker.GetTasks();
	ASSERT_EQ(tasksAfterMark.size(), tasksBeforeMark.size());
	for (uint32_t id = 0; id < tasksAfterMark.size(); id++)
	{
		ASSERT_NE(tasksAfterMark.find(id), tasksAfterMark.end());
		EXPECT_EQ(tasksAfterMark[id].desc, tasksBeforeMark[id].desc);
		EXPECT_EQ(tasksAfterMark[id].status, TASK_STATUS::IN_PROGRESS);
		EXPECT_EQ(tasksAfterMark[id].createdAt, tasksBeforeMark[id].createdAt);
		auto tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(taskMarkTimes[id] - tasksAfterMark[id].updatedAt);
		long long timeDiff = std::abs(tpDiff.count());
		EXPECT_LT(timeDiff, 100);
	}
}

TEST(MarkTask, MarkAllDone)
{
	std::vector<TASK> sampleTasks = {
		{ "NewTask1", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask2", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask3", TASK_STATUS::IN_PROGRESS, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
	};
	TaskTracker taskTracker(sampleTasks);
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	std::unordered_map<uint32_t, TASK> tasksBeforeMark = taskTracker.GetTasks();
	ASSERT_EQ(tasksBeforeMark.size(), sampleTasks.size());

	std::vector<std::chrono::system_clock::time_point> taskMarkTimes;
	for (uint32_t id = 0; id < tasksBeforeMark.size(); id++)
	{
		ASSERT_TRUE(taskTracker.MarkTask(id, TASK_STATUS::DONE));
		taskMarkTimes.push_back(std::chrono::system_clock::now());
	}

	std::unordered_map<uint32_t, TASK> tasksAfterMark = taskTracker.GetTasks();
	ASSERT_EQ(tasksAfterMark.size(), tasksBeforeMark.size());
	for (uint32_t id = 0; id < tasksAfterMark.size(); id++)
	{
		ASSERT_NE(tasksAfterMark.find(id), tasksAfterMark.end());
		EXPECT_EQ(tasksAfterMark[id].desc, tasksBeforeMark[id].desc);
		EXPECT_EQ(tasksAfterMark[id].status, TASK_STATUS::DONE);
		EXPECT_EQ(tasksAfterMark[id].createdAt, tasksBeforeMark[id].createdAt);
		auto tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(taskMarkTimes[id] - tasksAfterMark[id].updatedAt);
		long long timeDiff = std::abs(tpDiff.count());
		EXPECT_LT(timeDiff, 100);
	}
}

TEST(TestTaskTracker, GetTasksByStatus)
{
	std::vector<TASK> sampleTasks = {
		{ "NewTask1", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask2", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
		{ "NewTask3", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() },
	};
	TaskTracker taskTracker(sampleTasks);
	ASSERT_FALSE(taskTracker.IsJsonLoaded());
	ASSERT_EQ(taskTracker.GetTasks().size(), sampleTasks.size());

	ASSERT_TRUE(taskTracker.MarkTask(0, TASK_STATUS::DONE));
	ASSERT_TRUE(taskTracker.MarkTask(1, TASK_STATUS::IN_PROGRESS));

	TASK_STATUS targetStatus = TASK_STATUS::DONE;
	std::unordered_map<uint32_t, TASK> tasksByTargetStatus = taskTracker.GetTasks(&targetStatus);
	ASSERT_TRUE(tasksByTargetStatus.size(), 1);
	EXPECT_EQ(tasksByTargetStatus[0].status, TASK_STATUS::DONE);

	targetStatus = TASK_STATUS::IN_PROGRESS;
	tasksByTargetStatus = taskTracker.GetTasks(&targetStatus);
	ASSERT_TRUE(tasksByTargetStatus.size(), 1);
	EXPECT_EQ(tasksByTargetStatus[1].status, TASK_STATUS::IN_PROGRESS);
}

TEST(TestTaskTracker, ReadNonExistFile)
{
	std::string jsonFile = "nonexist.json";
	std::remove(jsonFile.c_str());

	TaskTracker* taskTracker = new TaskTracker(jsonFile);
	ASSERT_FALSE(taskTracker->IsJsonLoaded());
	EXPECT_TRUE(taskTracker->GetTasks().empty());
	delete taskTracker;
	taskTracker = nullptr;

	std::remove(jsonFile.c_str());
}

TEST(TestTaskTracker, ReadEmptyFile)
{
	std::string jsonFile = "testempty.json";
	std::ofstream outFile(jsonFile);
	outFile.close();
	ASSERT_FALSE(outFile.fail());

	TaskTracker taskTracker(jsonFile);
	EXPECT_TRUE(taskTracker.IsJsonLoaded());
	EXPECT_TRUE(taskTracker.GetTasks().empty());
}

TEST(TestTaskTracker, SaveAndReadFile)
{
	std::string jsonFile = "testsample.json";
	std::ofstream outFile(jsonFile);
	outFile.close();
	ASSERT_FALSE(outFile.fail());

	TaskTracker* taskTracker = new TaskTracker(jsonFile);
	ASSERT_TRUE(taskTracker->IsJsonLoaded());
	ASSERT_TRUE(taskTracker->GetTasks().empty());

	std::vector<std::string> samplesTaskDesc = { "ReadTask1", "ReadTask2", "ReadTask3" };
	for (const std::string& taskDesc : samplesTaskDesc)
	{
		taskTracker->AddTask(taskDesc);
	}

	std::unordered_map<uint32_t, TASK> sampleTasks = taskTracker->GetTasks();
	ASSERT_EQ(sampleTasks.size(), samplesTaskDesc.size());
	delete taskTracker;
	taskTracker = nullptr;

	TaskTracker taskTrackerRead(jsonFile);
	std::unordered_map<uint32_t, TASK> readTasks = taskTrackerRead.GetTasks();

	for (auto itr = readTasks.begin(); itr != readTasks.end(); itr++)
	{
		ASSERT_NE(sampleTasks.find(itr->first), sampleTasks.end());
		EXPECT_EQ(itr->second.desc, sampleTasks[itr->first].desc);
		EXPECT_EQ(itr->second.status, sampleTasks[itr->first].status);
		EXPECT_EQ(itr->second.createdAt, StringToTimePoint(TimePointToString(sampleTasks[itr->first].createdAt)));
		EXPECT_EQ(itr->second.updatedAt, StringToTimePoint(TimePointToString(sampleTasks[itr->first].updatedAt)));
	}
}