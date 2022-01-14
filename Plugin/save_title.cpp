#include "save_title.h"

namespace misc_patch
{
    std::unordered_map<std::size_t, std::vector<GTAChar>> truncate_map;
    std::unordered_map<std::size_t, std::vector<uchar>> expand_map;

    template <typename T>
    std::enable_if_t<std::is_integral_v<T>, std::size_t> fnv_1a(std::span<const T> seq)
    {
        static constexpr uint64 fnv_prime = 16777619ui64;

        std::size_t result = 2166136261u;

        for (auto& element : seq)
        {
            result ^= static_cast<uchar>(element);
            result = static_cast<uint>((result * fnv_prime) % 4294967296ui64);
        }

        return result;
    }

    //根据src的hash查找dst
    GTAChar* gtaTruncateString(uchar* dst, const GTAChar* src, unsigned size)
    {

        return nullptr;
    }

    void gtaExpandString(const uchar* src, GTAChar* dst)
    {
    }
}
