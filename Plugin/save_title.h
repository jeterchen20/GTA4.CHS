#pragma once
#include "../common/stdinc.h"

namespace misc_patch
{
    uchar* gtaTruncateString(uchar* dst, const GTAChar* str, unsigned size);
    void gtaExpandString(const uchar* src, GTAChar* dst);
}
