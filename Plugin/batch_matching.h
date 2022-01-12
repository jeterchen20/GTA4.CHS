#pragma once
#include "../common/stdinc.h"
#include "byte_pattern.h"

class batch_matching
{
public:
    typedef std::function<void(const byte_pattern::result_type &)> callback_type;
    typedef std::tuple<std::string, std::size_t, callback_type, byte_pattern::result_type> step_type;

    void register_step(const char *pattern, std::size_t desired_size, callback_type callback);
    void clear();
    void perform_search();
    bool is_all_succeed() const;
    void run_callbacks();

private:
    static constexpr int thread_count = 4;

    std::vector<step_type> _steps;
};
