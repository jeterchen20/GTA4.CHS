#pragma once

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;

//SparkIV
enum ResourceType :uint
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

enum CompressionType :ushort
{
    LZX = 0xF505,
    Deflate = 0xDA78
};

struct RSCHeader
{
    uint magic;
    ResourceType rtype;
    uint flags;
    CompressionType ctype;
};

//SparkIV.ResourceFile.cs
