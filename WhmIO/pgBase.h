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
    uint offset : 28; //��λ
    uint type : 4;    //��λ��5��ʾ�ڴ�����(������)��6��ʾ�Դ�����(��ͼ��vertex�ȵ�)

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

//ָ��һ��C String
struct pgPtr_String : pgPtr<char>
{
    std::string read(const buffer_type &buffer) const
    {
        return std::string(reinterpret_cast<const char *>(&buffer[offset]));
    }
};

//ָ��һ��DWORD
struct pgPtr_uint : pgPtr<uint>
{

};

//���飬ռ�ݵĿռ���size��count�Ľϴ���
template <typename T>
struct pgMaxArray : pgPtr<T>
{
    ushort count;
    ushort size;
};

//һ�����飬ռ�ݵĿռ���count
template <typename T>
struct pgCountArray : pgPtr<T>
{
    ushort count;
    ushort size;
};

//һ�����飬ռ�ݵĿռ���size
template <typename T>
struct pgSizeArray : pgPtr<T>
{
    ushort count;
    ushort size;
};
