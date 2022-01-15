#include "save_title.h"

namespace misc_patch
{
    std::unordered_map<std::size_t, std::vector<GTAChar>> string_map; //key�ǵ��ֽ��ַ�����hash, value��ԭʼ���ַ���������

    template <typename T>
    std::enable_if_t<std::is_integral_v<T>, std::size_t> fnv_1a(std::span<T> seq)
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

    template <typename T>
    std::enable_if_t<std::is_integral_v<T>, std::span<const T>> get_string_span(const T* str)
    {
        std::size_t size = 0;

        if (str != nullptr)
        {
            auto c = str[size];

            while (c != 0)
            {
                ++size;
                c = str[size];
            }
        }

        return { str,size };
    }

    template <typename T>
    std::enable_if_t<std::is_integral_v<T>, std::vector<T>> get_string_vector(const T* str)
    {
        std::vector<T> result;
        auto string_span = get_string_span(str);

        result.assign(string_span.begin(), string_span.end());

        return result;
    }

    template <typename T>
    std::enable_if_t<std::is_integral_v<T>, std::size_t> hash_string(const T* str)
    {
        return fnv_1a(get_string_span(str));
    }

    //8C5510
    uchar* gtaTruncateString(uchar* dst, const GTAChar* src, unsigned size)
    {
        std::size_t copied_size = 0;

        if (src != nullptr)
        {
            if (size > 1)
            {
                while (copied_size < size - 1)
                {
                    auto c = src[copied_size];

                    if (c == 0)
                        break;

                    //������ֽ�ǡ����0���������һ��������䣬��ֹ����г��������0
                    //��ϷҲֻ���ý�������ַ����Ƚϣ�Ӧ��û������
                    if ((c & 0xFF) == 0)
                        c |= 0x43;

                    dst[copied_size] = static_cast<uchar>(c);
                    ++copied_size;
                }

                //��dst��hash��src�����ݴ���map����gtaExpandString����
                //ע��Ҫ��ʵ�ʸ��Ƶĳ���ȡdst
                string_map.emplace(fnv_1a(std::span(dst, copied_size)), get_string_vector(src));
            }

            dst[copied_size] = 0;
        }

        return dst;
    }

    //909F50
    void gtaExpandString(const uchar* src, GTAChar* dst)
    {
        if (src == nullptr || dst == nullptr)
            return;

        auto wide_string_it = string_map.find(hash_string(src));

        if (wide_string_it != string_map.end())
        {
            std::ranges::copy(wide_string_it->second, dst);
            dst[wide_string_it->second.size()] = 0;
        }
        else
        {
            auto src_span = get_string_span(src);
            std::ranges::copy(src_span, dst);
            dst[src_span.size()] = 0;
        }
    }
}
