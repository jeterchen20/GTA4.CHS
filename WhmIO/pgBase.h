#pragma once
#include <cstdio>
#include <iterator>
#include <string>
#include <vector>
#include <stdexcept>
#include <d3d9.h>

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef std::vector<uchar> buffer_type;

enum ptr_element_type :uint
{
    Cpu_Type = 5,
    Gpu_Type = 6
};

template <typename T>
struct pgPtr
{
    uint offset : 28; //低位
    uint type : 4;    //高位，5表示内存内容(对象本身)，6表示显存内容(贴图，vertex等等)

    T* get_pointer(buffer_type& buffer) const
    {
        return reinterpret_cast<T*>(&buffer[offset]);
    }

    const T* get_pointer(const buffer_type& buffer) const
    {
        return reinterpret_cast<const T*>(&buffer[offset]);
    }

    template <typename CustomType>
    CustomType* get_pointer(buffer_type& buffer) const
    {
        return reinterpret_cast<CustomType*>(&buffer[offset]);
    }

    template <typename CustomType>
    const CustomType* get_pointer(const buffer_type& buffer) const
    {
        return reinterpret_cast<const CustomType*>(&buffer[offset]);
    }
};

//指向一个C String
struct pgPtr_String : pgPtr<char>
{
    std::string read(const buffer_type &buffer) const
    {
        return std::string(reinterpret_cast<const char *>(&buffer[offset]));
    }
};

//指向一个DWORD
struct pgPtr_uint : pgPtr<uint>
{

};

//数组，占据的空间是size和count的较大者
template <typename T>
struct pgMaxArray : pgPtr<T>
{
    ushort count;
    ushort size;
};

//一个数组，占据的空间是count
template <typename T>
struct pgCountArray : pgPtr<T>
{
    ushort count;
    ushort size;
};

//一个数组，占据的空间是size
template <typename T>
struct pgSizeArray : pgPtr<T>
{
    ushort count;
    ushort size;
};
