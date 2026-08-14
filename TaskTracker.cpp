#include "TaskTracker.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

const struct { TASK_STATUS status; std::string name; } TASKSTATUSMAP[] = {
	{TASK_STATUS::TODO, "todo"},
	{TASK_STATUS::IN_PROGRESS, "in-progress"},
	{TASK_STATUS::DONE, "done"}
};

std::string EnumToString(TASK_STATUS taskStatus)
{
	return TASKSTATUSMAP[taskStatus].name;
}

bool StringToEnum(const std::string& inString, TASK_STATUS& outStatus)
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

std::string TimePointToString(const std::chrono::system_clock::time_point& timePoint)
{
	std::time_t ctimeTimePoint = std::chrono::system_clock::to_time_t(timePoint);
	std::tm tmTimePoint = { 0 };
	localtime_s(&tmTimePoint, &ctimeTimePoint);

	std::stringstream ss;
	ss << std::put_time(&tmTimePoint, "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

std::chrono::system_clock::time_point StringToTimePoint(const std::string& tpString)
{
	std::stringstream ss(tpString);
	std::tm tmTimePoint = { 0 };
	ss >> std::get_time(&tmTimePoint, "%Y-%m-%d %H:%M:%S");

	std::time_t ctimeTimePoint = std::mktime(&tmTimePoint);
	return std::chrono::system_clock::from_time_t(ctimeTimePoint);
}

TaskTracker::TaskTracker(std::string readJsonPath)
	: jsonPath(readJsonPath)
{
	std::string jsonString;
	if (!ReadFileToString(jsonPath, jsonString))
	{
		jsonLoaded = false;
		return;
	}

	std::string backupFilePath = jsonPath + ".bak";
	WriteStringToFile(backupFilePath, jsonString);
	
	ParseJsonString(jsonString);
	jsonLoaded = true;
} 

TaskTracker::~TaskTracker()
{
	std::string tmpFilePath = jsonPath + ".tmp";
	std::string jsonString = Stringify();
	if (!WriteStringToFile(tmpFilePath, jsonString))
	{
		std::cerr << "Unable to create temporary file " << tmpFilePath << ", program failed to write modified task into file.\n";
		return;
	}

	std::remove(jsonPath.c_str());
	if (std::rename(tmpFilePath.c_str(), jsonPath.c_str()) != 0)
	{
		std::cerr << "Failed to rename into " << jsonPath << ", tasks remain saved in " << tmpFilePath << ".\n";
	}
}

bool TaskTracker::AddTask(std::string newDesc)
{
	if (tasks.size() == UINT32_MAX) { return false; }

	TASK newTask = { 
		tasks.size(),
		newDesc, 
		TASK_STATUS::TODO, 
		std::chrono::system_clock::now(), 
		std::chrono::system_clock::now() 
	};
	tasks.push_back(newTask);
	return true;
}

bool TaskTracker::UpdateTask(uint32_t id, std::string updatedDesc)
{
	if (id >= tasks.size()) { return false; }

	tasks.at(id).desc = updatedDesc;
	tasks.at(id).updatedAt = std::chrono::system_clock::now();
	return true;
}

bool TaskTracker::DeleteTask(uint32_t id)
{
	if (id >= tasks.size()) { return false; }

	for (int i = id + 1; i < tasks.size(); i++)
	{
		tasks.at(i - 1) = tasks.at(i);
		tasks.at(i - 1).id = i - 1;
	}
	tasks.pop_back();
	return true;
}

bool TaskTracker::MarkTask(uint32_t id, TASK_STATUS newStatus)
{
	if (id >= tasks.size()) { return false; }

	tasks.at(id).status = newStatus;
	tasks.at(id).updatedAt = std::chrono::system_clock::now();
	return true;
}

std::vector<TASK> TaskTracker::GetTasks(TASK_STATUS* targetStatus) const
{
	if (!targetStatus) { return tasks; }

	std::vector<TASK> targetStatusTasks;
	for (TASK task : tasks)
	{
		if (task.status == *targetStatus)
			targetStatusTasks.push_back(task);
	}
	return targetStatusTasks;
}

void TaskTracker::ParseJsonString(const std::string& jsonString)
{
	size_t searchOffset = 0;
	while (true)
	{
		size_t taskStartPos = jsonString.find('{', searchOffset);
		if (taskStartPos == std::string::npos) { break; }

		size_t taskEndPos = jsonString.find('}', taskStartPos + 1);
		if (taskEndPos == std::string::npos) { break; }

		searchOffset = taskEndPos + 1;

		TASK newTask = { 0 };
		std::string taskString = jsonString.substr(taskStartPos, taskEndPos - taskStartPos + 1);

		std::string keyValueString = "";
		if (!GetJsonKeyValue(taskString, "id", keyValueString)) { continue; }
		if (!ParseStringToUInt32(keyValueString, newTask.id)) { continue; }

		if (!GetJsonKeyValue(taskString, "desc", keyValueString)) { continue; }
		newTask.desc = GetCleanString(keyValueString);

		if (!GetJsonKeyValue(taskString, "status", keyValueString)) { continue; }
		keyValueString = GetCleanString(keyValueString);
		StringToEnum(keyValueString, newTask.status);

		if (!GetJsonKeyValue(taskString, "createdAt", keyValueString)) { continue; }
		keyValueString = GetCleanString(keyValueString);
		newTask.createdAt = StringToTimePoint(keyValueString);

		if (!GetJsonKeyValue(taskString, "updatedAt", keyValueString)) { continue; }
		keyValueString = GetCleanString(keyValueString);
		newTask.updatedAt = StringToTimePoint(keyValueString);

		tasks.push_back(newTask);
	}
}

std::string TaskTracker::Stringify()
{
	std::string jsonString = "[\n";
	for (size_t i = 0; i < tasks.size(); i++)
	{
		jsonString.append("\t{\n");
		jsonString.append("\t\t\"id\": " + std::to_string(tasks[i].id) + ",\n");
		jsonString.append("\t\t\"desc\": \"" + tasks[i].desc + "\",\n");
		jsonString.append("\t\t\"status\": \"" + EnumToString(tasks[i].status) + "\",\n");
		jsonString.append("\t\t\"createdAt\": \"" + TimePointToString(tasks[i].createdAt) + "\",\n");
		jsonString.append("\t\t\"updatedAt\": \"" + TimePointToString(tasks[i].updatedAt) + "\"\n");

		if (i != tasks.size() - 1) { jsonString.append("\t},\n"); }
		else { jsonString.append("\t}\n"); }
	}
	jsonString.append("]\n");
	return jsonString;
}