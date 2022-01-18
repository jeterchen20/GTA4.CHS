#pragma once
#include "../common/stdinc.h"

namespace string_util
{
    //字符串哈希函数
    std::size_t hash_string(const std::string& str);
    std::size_t hash_string(const char* str);
    std::size_t hash_string(const GTAChar *str);

    //非邮件
    uchar* gtaTruncateString(uchar* dst, const GTAChar* src, unsigned size);
    void gtaExpandString(const uchar* src, GTAChar* dst);

    //邮件
    void gtaTruncateString2(const GTAChar* src, uchar* dst);
    void gtaExpandString2(const uchar* src, GTAChar* dst);
}
