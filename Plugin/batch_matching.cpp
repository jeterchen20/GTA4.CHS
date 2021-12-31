#include "batch_matching.h"

#include <thread>
#include <future>

void batch_matching::register_step(const char* pattern, std::size_t desired_size, callback_type callback)
{
    _steps.emplace_back(pattern, desired_size, callback, container_type{});
}

void batch_matching::clear()
{
    _steps.clear();
}

void batch_matching::perform_search()
{
    std::vector<std::thread> pattern_threads;

    for (auto& step : _steps)
    {
        auto search_proc = [&step]() {
            byte_pattern pattern_obj;

            pattern_obj.find_pattern(std::get<0>(step).c_str());
            std::get<container_type>(step) = pattern_obj.get();
        };

        pattern_threads.emplace_back(search_proc);
    }

    for (auto& thread : pattern_threads)
    {
        thread.join();
    }
}

bool batch_matching::is_all_succeed() const
{
    return std::all_of(_steps.begin(), _steps.end(),
        [](const step_type& step)
        {
            return std::get<std::size_t>(step) == std::get<container_type>(step).size();
        });
}

void batch_matching::run_callbacks()
{
    for (auto& step : _steps)
    {
        std::get<callback_type>(step)(std::get<container_type>(step));
    }
}
