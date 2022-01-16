﻿#pragma once

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
#include <unordered_set>
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
#include <range/v3/algorithm.hpp>

typedef std::uint16_t GTAChar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long uint64;

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
    bool b;

    explicit stack_var(int val) { i = val; }
    explicit stack_var(void* val) { p = val; }
    explicit stack_var(float val) { f = val; }
    explicit stack_var(unsigned int val) { u = val; }
    explicit stack_var(bool val) { b = val; }

    stack_var& operator=(int val) { i = val; return *this; }
    stack_var& operator=(void* val) { p = val; return *this; }
    stack_var& operator=(float val) { f = val; return *this; }
    stack_var& operator=(unsigned int val) { u = val; return *this; }
    stack_var& operator=(bool val) { b = val; return *this; }
};
VALIDATE_SIZE(stack_var, 4);

inline std::filesystem::path relative_to_executable(HMODULE m, const std::filesystem::path& rest)
{
    wchar_t c_path[512];

    ::GetModuleFileNameW(m, c_path, 512);
    std::filesystem::path cpp_path{ c_path };
    return cpp_path.parent_path() / rest;
}

inline bool IsNativeChar(GTAChar c)
{
    return c < 0x100;
}

inline bool IsNormalNativeChar(GTAChar c)
{
    return IsNativeChar(c) && (c != 0) && (c != ' ') && (c != '~');
}
