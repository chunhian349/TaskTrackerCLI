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
	unsigned int id;
	std::string desc;
	TASK_STATUS status;
	TimePoint createdAt;
	TimePoint updatedAt;
};

class TaskTracker
{
private:
	std::vector<TASK> tasks;
	void ParseJsonString(const std::string& jsonString);
	
public:
	TaskTracker();
	~TaskTracker();
	void AddTask(std::string newDesc);
	void UpdateTask(unsigned int id, std::string updatedDesc);
	void DeleteTask(unsigned int id);
	void MarkTask(unsigned int id, TASK_STATUS newStatus);
	void ListTask(bool enableStatusFilter, TASK_STATUS targetStatus=TASK_STATUS::TODO);
};