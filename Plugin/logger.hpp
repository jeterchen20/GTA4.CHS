#pragma once
#include "../common/stdinc.h"

class logger
{
public:
    bool start_log(const std::filesystem::path& filename)
    {
        stop_log();

        _stream.open(filename, std::ios::trunc);

        return _stream.is_open();
    }

    void stop_log()
    {
        _stream.close();
    }

    void log_line(const std::string& text)
    {
        if (_stream)
            _stream << text << '\n';

        stop_log();
    }

    static logger &convience_instance()
    {
        static logger instance;

        return instance;
    }

private:
    std::ofstream _stream;
};
