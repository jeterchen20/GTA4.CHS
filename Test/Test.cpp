#include "../Plugin/save_title.h"

int main()
{
    std::array<std::thread, 4> search_threads;

    for (auto& thread : search_threads)
    {
        thread = std::thread([]{});
    }

    for (auto& thread : search_threads)
    {
        thread.join();
    }

    return 0;
}
