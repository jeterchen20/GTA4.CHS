#pragma once
#include "../common/stdinc.h"

enum rsc_type :uint
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

//block��С��256n
//block��������page
struct rsc_blocks
{
    std::vector<uchar> virtual_block;
    std::vector<uchar> physical_block;
};

//page��λ��С��2^n
struct rsc_flag
{
    union
    {
        struct
        {
            uint virtual1xPageFlag : 1;     //4��Сҳ��Ŀ���
            uint virtual2xPageFlag : 1;
            uint virtual4xPageFlag : 1;
            uint virtual8xPageFlag : 1;
            uint virtual16xPageCount : 7;   //��ҳ��ĸ���
            uint virtual1xPageSize : 4;     //���ھ�����λҳ��С((2^virtual1xPageSize)*256)

            uint physical1xPageFlag : 1;
            uint physical2xPageFlag : 1;
            uint physical4xPageFlag : 1;
            uint physical8xPageFlag : 1;
            uint physical16xPageCount : 7;
            uint physical1xPageSize : 4;

            uint useless : 2;
        };

        uint flags;
    }u;

    uint GetVirtualPageSize() const
    {
        return (1u << (u.virtual1xPageSize + 8u));
    }

    uint GetPhysicalPageSize() const
    {
        return (1u << (u.physical1xPageSize + 8u));
    }

    uint GetVirtualSize() const
    {
        return GetVirtualPageSize() * (
            u.virtual1xPageFlag * 1u +
            u.virtual2xPageFlag * 2u +
            u.virtual4xPageFlag * 4u +
            u.virtual8xPageFlag * 8u +
            u.virtual16xPageCount * 16u);
    }

    uint GetPhysicalSize() const
    {
        return GetPhysicalPageSize() * (
            u.physical1xPageFlag * 1u +
            u.physical2xPageFlag * 2u +
            u.physical4xPageFlag * 4u +
            u.physical8xPageFlag * 8u +
            u.physical16xPageCount * 16u);
    }
};
VALIDATE_SIZE(rsc_flag, 4);

//TODO: ��Ҫ����--��ȡ��д��ʱ�ı�pgPtr
struct rsc_header
{
    uint magic;
    uint type;
    rsc_flag flags;
};
VALIDATE_SIZE(rsc_header, 12);
