#include "logger.h"

std::ofstream Logger::_stream;

void Logger::StartLog(const char* filename)
{
    if (_stream)
        return;

    _stream.open(filename);
}

void Logger::LogLine(const std::string& text)
{
    if (!_stream)
        return;

    _stream << text << std::endl;
}

void Logger::StopLog()
{
    _stream.close();
}
