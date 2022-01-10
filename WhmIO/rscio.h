#pragma once
#include "../common/stdinc.h"

namespace rscio
{
    enum RscType :uint
    {
        TextureXBOX = 0x7, // xtd
        ModelXBOX = 0x6D, // xdr
        Generic = 0x01, // xhm / xad (Generic files as rsc?)
        Bounds = 0x20, // xbd, wbd
        Particles = 0x24, // xpfl
        Particles2 = 0x1B, // xpfl

        Texture = 0x8, // wtd
        Model = 0x6E, // wdr
        ModelFrag = 0x70, //wft
    };

    struct RscFlag
    {
        union
        {
            struct
            {
                uint cpu1xPageFlag : 1;     //4个小页面的开关
                uint cpu2xPageFlag : 1;
                uint cpu4xPageFlag : 1;
                uint cpu8xPageFlag : 1;
                uint cpu16xPageCount : 7;   //大页面的个数
                uint cpu1xPageSize : 4;     //用于决定单位页大小((2^cpu1xPageSize)*256)
                uint gpu1xPageFlag : 1;
                uint gpu2xPageFlag : 1;
                uint gpu4xPageFlag : 1;
                uint gpu8xPageFlag : 1;
                uint gpu16xPageCount : 7;
                uint gpu1xPageSize : 4;
                uint useless : 2;
            };

            uint flags;
        }u;

        uint GetCpuSize() const
        {
            auto page_size = (1u << (u.cpu1xPageSize + 8u));

            return page_size * (
                u.cpu1xPageFlag * 1u +
                u.cpu2xPageFlag * 2u +
                u.cpu4xPageFlag * 4u +
                u.cpu8xPageFlag * 8u +
                u.cpu16xPageCount * 16u);
        }

        uint GetGpuSize() const
        {
            auto page_size = (1u << (u.gpu1xPageSize + 8u));

            return page_size * (
                u.gpu1xPageFlag * 1u +
                u.gpu2xPageFlag * 2u +
                u.gpu4xPageFlag * 4u +
                u.gpu8xPageFlag * 8u +
                u.gpu16xPageCount * 16u);
        }
    };
    VALIDATE_SIZE(RscFlag, 4);

    struct RscHeader
    {
        uint magic;
        uint type;
        RscFlag flags;
        //uint flags;
    };
    VALIDATE_SIZE(RscHeader, 12);

    std::vector<uchar> read_rsc(const std::filesystem::path& filename);
    void write_rsc(const std::vector<uchar>& bytes, const std::filesystem::path& filename, uint virtual_size, uint physical_size);
}
