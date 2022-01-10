#pragma once
#include "pgBase.h"

struct pgPtr_OffsetMap :pgPtr<std::array<uint, 132>>
{

};

struct grcTexturePC
{
    uint vtbl;
    pgPtr_OffsetMap m_pOffsetMap;
    uchar    _f8;
    uchar    m_nbDepth;
    ushort    _fA;
    uint   _fC;
    uint   _f10;
    pgPtr_String    m_pszName;
    uint   m_piTexture;
    ushort    m_wWidth;
    ushort    m_wHeight;
    char    m_dwPixelFormat[4];
    ushort    m_wStride;
    uchar    m_eTextureType;
    uchar    m_nbLevels;
    D3DVECTOR   _f28;
    D3DVECTOR   _f34;
    uint   m_pNext;
    uint   m_pPrev;
    pgPtr<uchar>   m_pPixelData;
    uchar    _f4C;
    uchar    _f4D;
    uchar    _f4E;
    uchar    _f4F;
};

struct pgPtr_grcTexturePC :pgPtr<grcTexturePC>
{

};

struct pgSizeArray_uint :pgSizeArray<uint>
{

};

struct pgSizeArray_grcTexturePC :pgSizeArray<pgPtr_grcTexturePC>
{

};

struct pgDictionary_grcTexturePC
{
    uint           vtbl;
    pgPtr_OffsetMap m_pOffsetMap;
    uint           m_pParent;
    uint           m_dwUsageCount;
    pgSizeArray_uint   m_hashes;
    pgSizeArray_grcTexturePC    m_data;
};

struct pgPtr_pgDictionary_grcTexturePC :pgPtr<pgDictionary_grcTexturePC>
{

};
