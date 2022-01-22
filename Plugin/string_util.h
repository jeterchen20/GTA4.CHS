#pragma once
#include "../common/stdinc.h"

namespace string_util
{
    //字符串哈希函数
    std::size_t hash_string(const std::string& str, bool case_sens);
    std::size_t hash_string(const char* str, bool case_sens);
    std::size_t hash_string(const GTAChar* str, bool case_sens);

    //非邮件
    uchar* gtaTruncateString(uchar* dst, const GTAChar* src, unsigned size);
    void gtaExpandString(const uchar* src, GTAChar* dst);

    //邮件
    void gtaTruncateString2(const GTAChar* src, uchar* dst);
    void gtaExpandString2(const uchar* src, GTAChar* dst);

    //邮件回复
    //在utf8序列之间插入垃圾字节
    uchar* gtaTruncateString3(uchar* dst, const GTAChar* src, int size); //size为-1时则全部复制
}
