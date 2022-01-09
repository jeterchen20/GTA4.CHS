#pragma once
#include "../common/stdinc.h"
#include <zlib.h>

namespace rscio
{
    struct RscHeader
    {
        DWORD dwMagic;
        DWORD dwVersion;
        DWORD dwFlags;
    };
    static_assert(sizeof(RscHeader) == 12);

    std::vector<unsigned char> read_rsc(const std::filesystem::path& filename);
    void write_rsc(const std::vector<unsigned char>& bytes, const std::filesystem::path& filename);
}
