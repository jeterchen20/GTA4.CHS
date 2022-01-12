#pragma once
#include "../common/stdinc.h"
#include "rsc_header.h"

namespace rscio
{
    rsc_blocks read_rsc(const std::filesystem::path& filename);
    void write_rsc(const rsc_blocks& blocks, const std::filesystem::path& filename);
}
