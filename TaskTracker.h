#pragma once
#include <vector>
#include <string>
#include <chrono>

typedef std::chrono::system_clock::time_point TimePoint;
enum TASK_STATUS { TODO, IN_PROGRESS, DONE };

// Type conversion
std::string EnumToString(TASK_STATUS taskStatus);
bool StringToEnum(std::string inString, TASK_STATUS& outStatus);
std::string TimePointToString(TimePoint timePoint);
TimePoint StringToTimePoint(std::string tpString);

struct TASK
{
	uint32_t id;
	std::string desc;
	TASK_STATUS status;
	TimePoint createdAt;
	TimePoint updatedAt;
};

class TaskTracker
{
private:
	std::vector<TASK> tasks;
	std::string jsonFilePath;

	void ParseJsonString(const std::string& jsonString);
	std::string Stringify();
	
public:
	TaskTracker();
	~TaskTracker();
	void AddTask(std::string newDesc);
	void ListTask(bool enableStatusFilter, TASK_STATUS targetStatus=TASK_STATUS::TODO);
	void UpdateTask(uint32_t id, std::string updatedDesc);
	void DeleteTask(uint32_t id);
	void MarkTask(uint32_t id, TASK_STATUS newStatus);
};