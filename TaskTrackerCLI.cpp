// TaskTrackerCLI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "TaskTracker.h"
#include "Utils.h"
#include <iostream>
#include <unordered_map>

struct CLICOMMAND
{
   std::string command;
   std::vector<std::string> commandArgs;
};

static void AddTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() < 1)
   {
      std::cout << "Insufficient add task argument, need 1 have " << cli.commandArgs.size() << ".\n";
      return;
   }

   if (taskTracker.AddTask(cli.commandArgs[0]))
   {
      std::cout << "Task added successfully.\n";
   }
   else
   {
      std::cout << "Failed to add task, task size hit program max capacity.\n";
   }
}

static void UpdateTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() < 2)
   {
      std::cout << "Insufficient update task argument, need 2 have " << cli.commandArgs.size() << ".\n";
      return;
   }

   uint32_t id = 0;
   if (!ParseStringToUInt32(cli.commandArgs[0], id))
   {
      std::cout << "Update task failed, invalid id entered.\n";
      return;
   }

   if (taskTracker.UpdateTask(id, cli.commandArgs[1]))
   {
      std::cout << "Task updated successfully.\n";
   }
   else
   {
      std::cout << "Update task failed, invalid id entered.\n";
   }
}

static void DeleteTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() < 1)
   {
      std::cout << "Insufficient delete task argument, need 1 have " << cli.commandArgs.size() << ".\n";
      return;
   }
   
   uint32_t id = 0;
   if (!ParseStringToUInt32(cli.commandArgs[0], id))
   {
      std::cout << "Delete task failed, invalid id entered.\n";
      return;
   }

   if (taskTracker.DeleteTask(id))
   {
      std::cout << "Task deleted successfully.\n";
   }
   else
   {
      std::cout << "Delete task failed, invalid task id entered.\n";
   }
}

static void MarkTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() < 1)
   {
      std::cout << "Insufficient mark task argument, need 1 have " << cli.commandArgs.size() << ".\n";
      return;
   }

   uint32_t id = 0;
   if (!ParseStringToUInt32(cli.commandArgs[0], id))
   {
      std::cout << "Mark task failed, invalid id entered.\n";
      return;
   }

   TASK_STATUS newStatus = TODO;
   std::string findString = "mark-";
   size_t findPos = cli.command.find(findString);
   if (findPos == std::string::npos)
   {
      std::cout << "Mark task failed, unable to find \"mark-\" in command.\n";
      return;
   }

   if (!StringToEnum(cli.command.substr(findPos + findString.length()), newStatus))
   {
      std::cout << "Mark task failed, invalid status entered.\n";
      return;
   }

   taskTracker.MarkTask(id, newStatus);
}

static void ListTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   std::vector<TASK> tasks;
   if (cli.commandArgs.size() == 0)
   {
      tasks = taskTracker.GetTasks();
   }
   else
   {
      TASK_STATUS listStatus = TODO;
      if (!StringToEnum(cli.commandArgs[0], listStatus))
      {
         std::cout << "List task failed, invalid status entered.\n";
         return;
      }
      tasks = taskTracker.GetTasks(&listStatus);
   }

   if (tasks.empty())
      std::cout << "Task list is empty.\n";
      
   std::cout << "=========================Task list=========================\n\n";
   for (TASK task : tasks)
   {
      std::string strDateCreatedAt = TimePointToString(task.createdAt);
      std::string strDateUpdatedAt = TimePointToString(task.updatedAt);
      
      std::cout << "Task id #" << task.id << "\n";
      std::cout << "Description: " << task.desc << "\n";
      std::cout << "Status: " << EnumToString(task.status) << "\n";
      std::cout << "Created at: " << strDateCreatedAt << "\n";
      std::cout << "Updated at: " << strDateUpdatedAt << "\n";
      std::cout << "\n";
   }
   std::cout << "===========================================================\n";
}

int main(int argc, char* argv[])
{
   std::string jsonPath = "tasks.json";
   TaskTracker taskTracker(jsonPath);
   if (taskTracker.IsJsonLoaded())
   {
      std::cout << "Read file " << jsonPath << " successfully.\n";
   }
   else
   {
      std::cout << "Unable to read file " << jsonPath << ", an empty task list will be created.\n";
   }

   CLICOMMAND cli;
   const std::unordered_map<std::string, void(*)(TaskTracker&, const CLICOMMAND&)> COMMANDFUNCTIONS = {
      {"add", AddTask},
      {"update", UpdateTask },
      {"delete", DeleteTask },
      {"mark-in-progress", MarkTask},
      {"mark-done", MarkTask},
      {"list", ListTask}
   };

   if (argc <= 1)
   {
      std::cout << "No command entered.\n";
      return 0;
   }

   cli.command = argv[1];
   for (int i = 2; i < argc; i++)
   {
      cli.commandArgs.push_back(argv[i]);
   }

   auto it = COMMANDFUNCTIONS.find(cli.command);
   if (it == COMMANDFUNCTIONS.end() || !it->second)
   {
      std::cout << "Invalid command entered.\n";
      return 0;
   }

   it->second(taskTracker, cli);
   return 0;
}