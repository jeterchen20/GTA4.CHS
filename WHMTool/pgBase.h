#pragma once

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;

//SparkIV.ResourceFile.cs
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
    //CompressionType ctype;
};

//type为5时，在文件中的Offset位置放着指向的对象，否则不读
//以下所有都是一样的逻辑
struct pgPtr
{
    int offset : 28;
    int type : 4;
};

//一个C风格字符串
struct pgPtr_String : pgPtr
{

};

//一个DWORD
struct pgPtr_DWORD : pgPtr
{

};

//一个pgPtr_DWORD数组，占据的空间是sCount和sSize的较大者*4
struct CPtrCollection : pgPtr
{
    ushort  sCount;
    ushort  sSize;
};

//一个pgPtr数组，占据的空间是size个对象的大小
struct pgObjectArray : pgPtr
{
    ushort  count;
    ushort  size;
};
