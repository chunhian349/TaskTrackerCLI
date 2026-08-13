#pragma once
#include <string>

// File Operations
/* Copy file text into string on success file read, else return false without modify string */
bool ReadFileToString(std::string filePath, std::string& outString);
/* Copy string into file on success file write, else return false without modify file */
bool WriteStringToFile(std::string filePath, const std::string& inString);

// JSON Utilities
/* Extract value of JSON key if key found inside jsonString and JSON key value in correct format.
	Support primitive key only (non-array and non-object) */ 
bool GetJsonKeyValue(std::string jsonString, std::string key, std::string& valueString);

// String Processing
/* Extract characters between quotation symbols ("") */
std::string GetCleanString(std::string inString);
/* Ignore preceding or trailing whitespaces.
	Fail to parse if there is non-numeric character or whitespace between numeric characters */
bool ParseStringToUInt32(std::string inString, uint32_t& outUInt);