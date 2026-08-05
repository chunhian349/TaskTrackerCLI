#include "TaskTracker.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

const struct { TASK_STATUS status; std::string name; } TASKSTATUSMAP[] = {
	{TASK_STATUS::TODO, "todo"},
	{TASK_STATUS::IN_PROGRESS, "in-progress"},
	{TASK_STATUS::DONE, "done"}
};

std::string EnumToString(TASK_STATUS taskStatus)
{
	return TASKSTATUSMAP[taskStatus].name;
}

bool StringToEnum(std::string inString, TASK_STATUS& outStatus)
{
	for (auto taskStatusMap : TASKSTATUSMAP)
	{
		if (inString == taskStatusMap.name)
		{
			outStatus = taskStatusMap.status;
			return true;
		}
	}
	return false;
}

std::string TimePointToString(TimePoint timePoint, std::string format)
{
	std::time_t ctimeTimePoint = std::chrono::system_clock::to_time_t(timePoint);
	std::tm* tmTimePoint = new tm;
	localtime_s(tmTimePoint, &ctimeTimePoint);

	std::stringstream ss;
	ss << std::put_time(tmTimePoint, format.c_str());
	delete tmTimePoint;
	return ss.str();
}

TaskTracker::TaskTracker()
{
	tasks.push_back({ 0, "Task1 Description", TASK_STATUS::TODO, std::chrono::system_clock::now(), std::chrono::system_clock::now() });
	tasks.push_back({ 1, "Task2 Description", TASK_STATUS::IN_PROGRESS, std::chrono::system_clock::now(), std::chrono::system_clock::now() });
	tasks.push_back({ 2, "Task3 Description", TASK_STATUS::DONE, std::chrono::system_clock::now(), std::chrono::system_clock::now() });
}

TaskTracker::~TaskTracker()
{

}

void TaskTracker::AddTask(std::string newDesc)
{
	TASK newTask = { 
		tasks.size(),
		newDesc, 
		TASK_STATUS::TODO, 
		std::chrono::system_clock::now(), 
		std::chrono::system_clock::now() 
	};
	tasks.push_back(newTask);
	std::cout << "Task added successfully." << std::endl;
}

void TaskTracker::UpdateTask(unsigned int id, std::string updatedDesc)
{
	if (id >= tasks.size())
	{
		std::cout << "Update task failed, invalid id entered" << std::endl;
		return;
	}

	tasks.at(id).desc = updatedDesc;
	tasks.at(id).updatedAt = std::chrono::system_clock::now();
	std::cout << "Task updated successfully." << std::endl;
}

void TaskTracker::DeleteTask(unsigned int id)
{
	if (id >= tasks.size())
	{
		std::cout << "Delete task failed, invalid task id entered." << std::endl;
		return;
	}

	for (int i = id + 1; i < tasks.size(); i++)
	{
		tasks.at(i - 1) = tasks.at(i);
		tasks.at(i - 1).id = i - 1;
	}
	
	tasks.pop_back();
	std::cout << "Task deleted successfully." << std::endl;
}

void TaskTracker::MarkTask(unsigned int id, TASK_STATUS newStatus)
{
	if (id >= tasks.size())
	{
		std::cout << "Mark task failed, invalid task id entered." << std::endl;
		return;
	}

	tasks.at(id).status = newStatus;
	tasks.at(id).updatedAt = std::chrono::system_clock::now();
	std::cout << "Task marked successfully." << std::endl;
}

void TaskTracker::ListTask(bool enableStatusFilter, TASK_STATUS targetStatus)
{
	if (tasks.empty())
		std::cout << "Task list is empty." << std::endl;

	std::cout << "=========================Task list=========================" << std::endl;
	for (TASK task : tasks)
	{
		if (enableStatusFilter && task.status != targetStatus)
			continue;

		std::string strDateCreatedAt = TimePointToString(task.createdAt, "%Y-%m-%d %H:%M:%S");
		std::string strDateUpdatedAt = TimePointToString(task.updatedAt, "%Y-%m-%d %H:%M:%S");

		std::cout << "Task id #" << task.id << std::endl;
		std::cout << "Description: " << task.desc << std::endl;
		std::cout << "Status: " << EnumToString(task.status) << std::endl;
		std::cout << "Created at: " << strDateCreatedAt << std::endl;
		std::cout << "Updated at: " << strDateUpdatedAt << std::endl;
		std::cout << std::endl;
	}
	std::cout << "===========================================================" << std::endl;
}