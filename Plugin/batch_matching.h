#pragma once
#include "../common/stdinc.h"
#include "byte_pattern.h"

class batch_matching
{
public:
    typedef std::function<void(const byte_pattern::result_type &)> callback_type;

    struct match_step
    {
        std::string pattern;
        std::size_t expected_size;
        callback_type callback;
        byte_pattern::result_type result;
    };

    void register_step(const char *pattern, std::size_t desired_size, callback_type callback);
    void clear();
    void perform_search();
    bool is_all_succeed() const;
    void run_callbacks() const;

private:
    std::vector<match_step> _steps;
};
