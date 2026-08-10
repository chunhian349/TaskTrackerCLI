// TaskTrackerCLI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "TaskTracker.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>

struct CLICOMMAND
{
   std::string command;
   std::vector<std::string> commandArgs;
};

bool ParseStringToUInt(std::string inString, unsigned int& outUInt)
{
   if (!inString.length())
      return false;

   size_t invalidCharPos = inString.find_first_not_of("0123456789");
   if (invalidCharPos != std::string::npos)
      return false;

   std::stringstream ss(inString);
   ss >> outUInt;
   return true;
}

void AddTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() < 1)
   {
      std::cout << "Insufficient add task argument, need 1 have " << cli.commandArgs.size() << std::endl;
      return;
   }

   taskTracker.AddTask(cli.commandArgs[0]);
}

void UpdateTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() < 2)
   {
      std::cout << "Insufficient update task argument, need 2 have " << cli.commandArgs.size() << std::endl;
      return;
   }

   unsigned int id = 0;
   if (!ParseStringToUInt(cli.commandArgs[0], id))
   {
      std::cout << "Update task failed, invalid id entered" << std::endl;
      return;
   }

   taskTracker.UpdateTask(id, cli.commandArgs[1]);
}

void DeleteTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() < 1)
   {
      std::cout << "Insufficient delete task argument, need 1 have " << cli.commandArgs.size() << std::endl;
      return;
   }
   
   unsigned int id = 0;
   if (!ParseStringToUInt(cli.commandArgs[0], id))
   {
      std::cout << "Delete task failed, invalid id entered" << std::endl;
      return;
   }

   taskTracker.DeleteTask(id);
}

void MarkTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() < 1)
   {
      std::cout << "Insufficient mark task argument, need 1 have " << cli.commandArgs.size() << std::endl;
      return;
   }

   unsigned int id = 0;
   if (!ParseStringToUInt(cli.commandArgs[0], id))
   {
      std::cout << "Mark task failed, invalid id entered" << std::endl;
      return;
   }

   TASK_STATUS newStatus = TODO;
   std::string findString = "mark-";
   size_t findPos = cli.command.find(findString);
   if (findPos == std::string::npos)
      return;

   StringToEnum(cli.command.substr(findPos + findString.length()), newStatus);
   taskTracker.MarkTask(id, newStatus);
}

void ListTask(TaskTracker& taskTracker, const CLICOMMAND& cli)
{
   if (cli.commandArgs.size() == 0)
   {
      taskTracker.ListTask(false);
   }
   else
   {
      TASK_STATUS listStatus = TODO;
      if (!StringToEnum(cli.commandArgs[0], listStatus))
      {
         std::cout << "List task failed, invalid status entered." << std::endl;
         return;
      }
      taskTracker.ListTask(true, listStatus);
   }
}

int main(int argc, char *argv[])
{
   TaskTracker taskTracker;
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
      return 0;

   cli.command = argv[1];
   for (int i = 2; i < argc; i++)
   {
      cli.commandArgs.push_back(argv[i]);
   }

   auto it = COMMANDFUNCTIONS.find(cli.command);
   if (it == COMMANDFUNCTIONS.end() || !it->second)
   {
      std::cout << "Invalid command entered." << std::endl;
      return 0;
   }

   it->second(taskTracker, cli);
   return 0;
}