#include "batch_matching.h"

void batch_matching::register_step(const char* pattern, std::size_t desired_size, callback_type callback)
{
    _steps.emplace_back(pattern, desired_size, callback, byte_pattern::result_type{});
}

void batch_matching::clear()
{
    _steps.clear();
}

void batch_matching::perform_search()
{
    std::atomic_flag tasks_fetch_flag = ATOMIC_FLAG_INIT;
    std::size_t next_step_index = 0;

    auto search_proc = [&tasks_fetch_flag, &next_step_index, this]()
    {
        byte_pattern pattern_obj;

        while (true)
        {
            if (tasks_fetch_flag.test_and_set())
                continue;

            if (next_step_index >= _steps.size())
            {
                tasks_fetch_flag.clear();
                return;
            }

            auto my_index = next_step_index;
            ++next_step_index;
            tasks_fetch_flag.clear();

            pattern_obj.find_pattern(std::get<0>(_steps[my_index]).c_str());
            std::get<byte_pattern::result_type>(_steps[my_index]) = pattern_obj.get();
        }
    };

    std::vector search_threads(thread_count, std::thread(search_proc));

    for (auto& thread : search_threads)
    {
        thread.join();
    }
}

bool batch_matching::is_all_succeed() const
{
    return std::all_of(_steps.begin(), _steps.end(),
        [](const step_type& step)
        {
            return std::get<std::size_t>(step) == std::get<byte_pattern::result_type>(step).size();
        });
}

void batch_matching::run_callbacks()
{
    for (auto& step : _steps)
    {
        std::get<callback_type>(step)(std::get<byte_pattern::result_type>(step));
    }
}
