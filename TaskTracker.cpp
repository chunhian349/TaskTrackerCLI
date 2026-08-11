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

std::string TimePointToString(TimePoint timePoint)
{
	std::time_t ctimeTimePoint = std::chrono::system_clock::to_time_t(timePoint);
	std::tm tmTimePoint = { 0 };
	localtime_s(&tmTimePoint, &ctimeTimePoint);

	std::stringstream ss;
	ss << std::put_time(&tmTimePoint, "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

TimePoint StringToTimePoint(std::string tpString)
{
	std::stringstream ss(tpString);
	std::tm tmTimePoint = { 0 };
	ss >> std::get_time(&tmTimePoint, "%Y-%m-%d %H:%M:%S");

	std::time_t ctimeTimePoint = std::mktime(&tmTimePoint);
	return std::chrono::system_clock::from_time_t(ctimeTimePoint);
}

bool ReadFileToString(std::string filePath, std::string& outString)
{
	std::ifstream inFile(filePath);
	if (!inFile.is_open())
		return false;

	outString = { std::istreambuf_iterator<char>(inFile), {} };
	return true;
}

// Only for keys of primitive type (non-struct or non-array)
bool GetJsonKeyValue(std::string jsonString, std::string key, std::string& valueString)
{
	std::string stringKey = "\"" + key + "\"";
	size_t keyPos = jsonString.find(stringKey);
	if (keyPos == std::string::npos)
		return false;

	size_t colonPos = jsonString.find(':', keyPos + key.size());
	if (colonPos == std::string::npos)
		return false;

	size_t endSymbolPos = jsonString.find_first_of(",}", colonPos + 1);
	if (endSymbolPos == std::string::npos)
		return false;

	size_t valueStringSize = endSymbolPos - colonPos - 1;
	if (valueStringSize == 0)
		return false;

	valueString = jsonString.substr(colonPos + 1, valueStringSize);
	return true;
}

std::string GetCleanString(std::string inString)
{
	size_t startQuotePos = inString.find('"');
	if (startQuotePos == std::string::npos)
		return inString;

	size_t endQuotePos = inString.rfind('"');
	if (endQuotePos == std::string::npos || startQuotePos == endQuotePos)
		return inString;

	return inString.substr(startQuotePos + 1, endQuotePos - startQuotePos - 1);
}

bool WriteStringToFile(std::string filePath, const std::string& inString)
{
	std::ofstream outFile(filePath);
	if (!outFile.is_open())
		return false;

	outFile << inString;
	return true;
}

TaskTracker::TaskTracker()
{
	jsonFilePath = "tasks.json";
	std::string jsonString;
	if (!ReadFileToString(jsonFilePath, jsonString))
	{
		std::cout << "Unable to read file " << jsonFilePath << ", an empty task list will be created." << std::endl;
		return;
	}
	std::cout << "Read file " << jsonFilePath << " successfully." << std::endl;

	std::string backupFilePath = jsonFilePath + ".bak";
	if (!WriteStringToFile(backupFilePath, jsonString))
	{
		std::cout << "Unable to create backup file " << backupFilePath << " , program will proceed without backup file." << std::endl;
	}
	else
	{
		std::cout << "Create backup file " << backupFilePath << " successfully." << std::endl;
	}
	
	ParseJsonString(jsonString);
}

TaskTracker::~TaskTracker()
{
	std::string tmpFilePath = jsonFilePath + ".tmp";
	std::string jsonString = Stringify();
	if (!WriteStringToFile(tmpFilePath, jsonString))
	{
		std::cout << "Unable to create temporary file " << " , program failed to write modified task into file." << std::endl;
		return;
	}

	std::remove(jsonFilePath.c_str());
	if (std::rename(tmpFilePath.c_str(), jsonFilePath.c_str()) == 0)
	{
		std::cout << "Save tasks into " << jsonFilePath << " successfully." << std::endl;
	}
	else
	{
		std::cout << "Failed to rename into " << jsonFilePath << ", tasks remain saved in " << tmpFilePath << std::endl;
	}
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

		std::string strDateCreatedAt = TimePointToString(task.createdAt);
		std::string strDateUpdatedAt = TimePointToString(task.updatedAt);

		std::cout << "Task id #" << task.id << std::endl;
		std::cout << "Description: " << task.desc << std::endl;
		std::cout << "Status: " << EnumToString(task.status) << std::endl;
		std::cout << "Created at: " << strDateCreatedAt << std::endl;
		std::cout << "Updated at: " << strDateUpdatedAt << std::endl;
		std::cout << std::endl;
	}
	std::cout << "===========================================================" << std::endl;
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
		std::stringstream ss(keyValueString);
		ss >> newTask.id;

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