#include "batch_matching.h"

void batch_matching::register_step(const char* pattern, std::size_t desired_size, callback_type callback)
{
    _steps.emplace_back(pattern, desired_size, callback);
}

void batch_matching::clear()
{
    _steps.clear();
}

void batch_matching::perform_search()
{
    byte_pattern pattern_obj;

    for (auto& step : _steps)
    {
        pattern_obj.find_pattern(step.pattern.c_str());
        step.result = pattern_obj.get();
    }
}

bool batch_matching::is_all_succeed() const
{
    return ranges::all_of(_steps,
        [](const match_step& step)
        {
            return step.expected_size == step.result.size();
        });
}

void batch_matching::run_callbacks() const
{
    for (auto& step : _steps)
    {
        step.callback(step.result);
    }
}
