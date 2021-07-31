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

//typeΪ5ʱ�����ļ��е�Offsetλ�÷���ָ��Ķ��󣬷��򲻶�
//�������ж���һ�����߼�
struct pgPtr
{
    int offset : 28;
    int type : 4;
};

//һ��C����ַ���
struct pgPtr_String : pgPtr
{

};

//һ��DWORD
struct pgPtr_DWORD : pgPtr
{

};

//һ��pgPtr_DWORD���飬ռ�ݵĿռ���sCount��sSize�Ľϴ���*4
struct CPtrCollection : pgPtr
{
    ushort  sCount;
    ushort  sSize;
};

//һ��pgPtr���飬ռ�ݵĿռ���size������Ĵ�С
struct pgObjectArray : pgPtr
{
    ushort  count;
    ushort  size;
};
