#pragma once
#include <cstdio>
#include <iterator>
#include <string>
#include <vector>
#include <stdexcept>
#include <d3d9.h>
#include "../common/MemoryFile.hpp"

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;

enum class ptr_element_type :uint
{
    Cpu_Type = 5,
    Gpu_Type = 6
};

//二进制文件中的指针
template <typename T, ptr_element_type type = ptr_element_type::Cpu_Type>
struct pgPtr
{
    union
    {
        struct
        {
            uint offset : 28; //低位
            ptr_element_type type : 4;    //高位，5表示内存内容(对象本身)，6表示显存内容(贴图，vertex等等)，显存内容不需要读
        };

        uint p;
    }u;
    VALIDATE_SIZE(u, 4);
};

//指向一个C String
struct pgString : pgPtr<char>
{

};

//对象数组
template <typename T>
struct pgObjectArray : pgPtr<T>
{
    ushort count; //有效元素个数(std::vector的size)
    ushort size;  //占据空间个数(std::vector的capacity)
};

//对象指针数组
template <typename T>
struct pgPtrArray : pgObjectArray<pgPtr<T>>
{
};
