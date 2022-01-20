#pragma once
#include "pgBase.h"

struct pgPtr_OffsetMap :pgPtr<std::array<uint, 132>>
{

};

struct grcTexturePC
{
    virtual ~grcTexturePC() = default;

    //uint vtbl;
    pgPtr_OffsetMap m_pOffsetMap;
    uchar    _f8;
    uchar    m_nbDepth;
    ushort    _fA;
    uint   _fC;
    uint   _f10;
    pgString    m_pszName;
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

struct pgDictionary_grcTexturePC
{
    uint           vtbl;
    pgPtr_OffsetMap m_pOffsetMap;
    uint           m_pParent;
    uint           m_dwUsageCount;
    pgObjectArray<uint>   m_hashes;
    pgObjectArray<grcTexturePC>    m_data;
};
