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
#include "injector/hooking.hpp"
#include "injector/calling.hpp"
#include "injector/injector.hpp"
#include "BinaryFile.hpp"

#include "../common/tinyutf8.h"

//vcpkg
#include <fmt/printf.h>

typedef std::uint16_t GTAChar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

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
