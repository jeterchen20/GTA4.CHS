#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#pragma warning(disable:4244)

#define VALIDATE_SIZE(expr, size) static_assert(sizeof(expr) == size, "Wrong size.")

#include <Windows.h>
#include <d3d9.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <regex>
#include <fstream>
#include <functional>
#include <array>
#include <utility>
#include <map>
#include <set>
#include <unordered_map>
#include <string>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <variant>
#include <span>

#include "injector/hooking.hpp"
#include "injector/calling.hpp"
#include "injector/injector.hpp"
#include "BinaryFile.hpp"
#include "MemoryFile.hpp"

#include "../common/tinyutf8.h"

//vcpkg
#include <fmt/printf.h>
#include <zlib.h>

typedef std::uint16_t GTAChar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

struct CRect
{
    float fBottomLeftX;
    float fBottomLeftY;
    float fTopRightX;
    float fTopRightY;
};

struct CharacterPos
{
    uchar row, column;
};

struct CharacterData
{
    GTAChar code;
    CharacterPos pos;
};

union stack_var
{
    int i;
    void* p;
    float f;
    unsigned int u;

    explicit stack_var(int val) { i = val; }
    explicit stack_var(void* val) { p = val; }
    explicit stack_var(float val) { f = val; }
    explicit stack_var(unsigned int val) { u = val; }

    stack_var& operator=(int val) { i = val; }
    stack_var& operator=(void* val) { p = val; }
    stack_var& operator=(float val) { f = val; }
    stack_var& operator=(unsigned int val) { u = val; }

    operator int() const { return i; }
    operator void* () const { return p; }
    operator float() const { return f; }
    operator unsigned int() const { return u; }
};
VALIDATE_SIZE(stack_var, 4);

inline std::filesystem::path relative_to_executable(HMODULE m, const std::filesystem::path& rest)
{
    wchar_t c_path[512];

    ::GetModuleFileNameW(m, c_path, 512);
    std::filesystem::path cpp_path{ c_path };
    return cpp_path.parent_path() / rest;
}
