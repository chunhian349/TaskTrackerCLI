#include "Utils.h"
#include <fstream>

bool ReadFileToString(std::string filePath, std::string& outString)
{
	std::ifstream inFile(filePath);
	if (!inFile.is_open())
		return false;

	outString = { std::istreambuf_iterator<char>(inFile), {} };
	return true;
}

bool WriteStringToFile(std::string filePath, const std::string& inString)
{
	std::ofstream outFile(filePath);
	if (!outFile.is_open())
		return false;

	outFile << inString;
	outFile.flush();
	outFile.close();
	return !outFile.fail();
}

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

bool ParseStringToUInt32(std::string inString, uint32_t& outUInt)
{
	if (!inString.length())
		return false;

	std::string whitespaces = " \t\r\n";
	size_t numericFirstPos = inString.find_first_not_of(whitespaces);
	if (numericFirstPos == std::string::npos)
		return false;
	size_t numericLastPos = inString.find_last_not_of(whitespaces);
	std::string trimInString = inString.substr(numericFirstPos, numericLastPos - numericFirstPos + 1);

	char* endPtr = nullptr;
	unsigned long long val = std::strtoull(trimInString.c_str(), &endPtr, 10);
	if (*endPtr != '\0')
		return false;
	if (val > UINT32_MAX)
		return false;

	outUInt = static_cast<uint32_t>(val);
	return true;
}