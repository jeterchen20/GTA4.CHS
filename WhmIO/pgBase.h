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

enum ptr_element_type :uint
{
    Cpu_Type = 5,
    Gpu_Type = 6
};

struct pgStreamable
{


};

struct datBase :pgStreamable
{
    virtual ~datBase() = default;
};

//�������ļ��е�ָ��
template <typename T>
struct pgPtr
{
    union
    {
        struct
        {
            uint offset : 28; //��λ
            ptr_element_type type : 4;    //��λ��5��ʾ�ڴ�����(������)��6��ʾ�Դ�����(��ͼ��vertex�ȵ�)���Դ����ݲ���Ҫ��
        };

        uint p;
    }u;
    VALIDATE_SIZE(u, 4);

    T read(MemoryFile& file) const
    {
        T result;

        file.Seek(u.offset, SEEK_SET);
        file.Read(result);
        return result;
    }
};

//ָ��һ��C String
struct pgString : pgPtr<char>
{
    std::string read(MemoryFile& file) const
    {
        std::string result;

        file.Seek(u.offset, SEEK_SET);

        while (true)
        {
            char c;

            file.Read(c);

            if (c == 0)
                break;

            result.push_back(c);
        }

        return result;
    }
};

//��������
template <typename T>
struct pgObjectArray : pgPtr<T>
{
    ushort count; //��ЧԪ�ظ���(std::vector��size)
    ushort size;  //ռ�ݿռ����(std::vector��capacity)

    std::vector<T> read(MemoryFile& file) const
    {
        std::vector<T> result;

        //����T�ǿ���ֱ��memcpy������
        file.Seek(u.offset);
        result.resize(count);
        file.ReadArray(count, result);
    }
};

//����ָ������
template <typename T>
struct pgPtrArray : pgObjectArray<pgPtr<T>>
{
    std::vector<T> read(MemoryFile& file) const
    {
        std::vector<T> result;

        auto ptrs = pgObjectArray<T>::read(file);

        for (auto &ptr:ptrs)
        {
            result.push_back(ptr.read());
        }

        return result;
    }
};
