#pragma once
#include <fstream>
#include <string>

class Logger
{
public:
    static void StartLog(const char* filename);
    static void LogLine(const std::string& text);
    static void StopLog();

private:
    static std::ofstream _stream;
};
