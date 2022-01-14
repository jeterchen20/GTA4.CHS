#pragma once
#include "../common/stdinc.h"

namespace misc_patch
{
    GTAChar* gtaTruncateString(uchar* dst, const GTAChar* str, unsigned size);
    void gtaExpandString(uchar* src, GTAChar* dst);
}
