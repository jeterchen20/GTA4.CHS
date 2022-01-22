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

void batch_matching::perform_search_mt()
{
    constexpr std::size_t thread_count = 4;

    std::atomic_flag tasks_fetch_flag = ATOMIC_FLAG_INIT;
    std::size_t free_step_index = 0;

    auto search_proc = [&tasks_fetch_flag, &free_step_index, this]()
    {
        byte_pattern pattern_obj;

        while (true)
        {
            if (tasks_fetch_flag.test_and_set(std::memory_order::memory_order_acq_rel))
                continue;

            if (free_step_index >= _steps.size())
            {
                break;
            }

            auto my_index = free_step_index;
            ++free_step_index;
            tasks_fetch_flag.clear(std::memory_order::memory_order_release); //拿到自己的任务后就要释放锁

            pattern_obj.find_pattern(_steps[my_index].pattern.c_str());
            _steps[my_index].result = pattern_obj.get();
        }

        tasks_fetch_flag.clear(std::memory_order::memory_order_release);
    };

    std::array<std::thread, thread_count> search_threads;

    for (auto& thread : search_threads)
    {
        thread = std::thread(search_proc);
    }

    for (auto& thread : search_threads)
    {
        thread.join();
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
