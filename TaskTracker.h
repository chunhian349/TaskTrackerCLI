#pragma once
#include <vector>
#include <string>
#include <chrono>

enum TASK_STATUS { TODO, IN_PROGRESS, DONE };

struct TASK
{
	uint32_t id;
	std::string desc;
	TASK_STATUS status;
	std::chrono::system_clock::time_point createdAt;
	std::chrono::system_clock::time_point updatedAt;
};

std::string EnumToString(TASK_STATUS taskStatus);
bool StringToEnum(const std::string& inString, TASK_STATUS& outStatus);
std::string TimePointToString(const std::chrono::system_clock::time_point& timePoint);
std::chrono::system_clock::time_point StringToTimePoint(const std::string& tpString);

class TaskTracker
{
private:
	std::vector<TASK> tasks;
	std::string jsonPath;
	bool jsonLoaded;

public:
	TaskTracker(std::string readJsonPath);
	~TaskTracker();
	bool IsJsonLoaded() const { return jsonLoaded; }
	bool AddTask(std::string newDesc);
	bool UpdateTask(uint32_t id, std::string updatedDesc);
	bool DeleteTask(uint32_t id);
	bool MarkTask(uint32_t id, TASK_STATUS newStatus);
	std::vector<TASK> GetTasks(TASK_STATUS* targetStatus=nullptr) const;

private:
	void ParseJsonString(const std::string& jsonString);
	std::string Stringify();
};