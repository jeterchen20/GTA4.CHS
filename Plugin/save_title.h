#pragma once
#include "../common/stdinc.h"

namespace misc_patch
{
    //���ʼ�
    uchar* gtaTruncateString(uchar* dst, const GTAChar* src, unsigned size);
    void gtaExpandString(const uchar* src, GTAChar* dst);

    //�ʼ�
    void gtaTruncateString2(const GTAChar* src, uchar* dst);
    void gtaExpandString2(const uchar* src, GTAChar* dst);
}
