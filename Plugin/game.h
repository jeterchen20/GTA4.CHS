#pragma once
#include "../common/stdinc.h"
#include "font.h"

struct StockAddresses
{
    //80 7C 24 08 00 A1
    void* pGraphics_SetRenderState;

    //85 C9 53 55 56 57 74 40 两个结果，选哪个都可以
    void* pDictionary_GetElementByKey;

    //8B 4C 24 08 56 8B 74 24 08 80 3E 22
    void* pHash_HashStringFromSeediCase;

    //A1 (? ? ? ?) 68 ? ? ? ? 8B 08 68 ? ? ? ? 6A 14
    IDirect3D9** ppDirect3D9;

    //A1 ? ? ? ? 68 (? ? ? ?) 8B 08 68 ? ? ? ? 6A 14
    IDirect3DDevice9** ppDirect3DDevice9;

    //8D 0C C5 (? ? ? ?) 51 8D
    CFontDetails* pFont_Details;
    //83 C6 2C 83 C7 01 81 FE (? ? ? ?)
    CFontRenderState* pFont_RenderState;
    //D8 35 (? ? ? ?) 5E D8 05
    float* pFont_ResolutionX;
    //A1 ? ? ? ? 83 F8 FF 75 1E
    void* pFont_GetRenderIndex;
    //83 EC 34 83 3D ? ? ? ? FF
    void* pFont_PrintChar;
    //83 EC 08 56 57 E8
    void* pFont_GetCharacterSizeNormal;
    //51 A0 ? ? ? ? 0F B6 15
    void* pFont_GetCharacterSizeDrawing;
    //83 EC 10 8B 44 24 14 F3 0F 7E 00
    void* pFont_Render2DPrimitive;

    void *pFont_ParseToken;
};

class CGame
{
public:
    static StockAddresses Addresses;

    static void Graphics_SetRenderState(void* texture, int = 1);

    static void* Dictionary_GetElementByKey(void* dictionary, uint hash);

    static uint Hash_HashStringFromSeediCase(const char* str, uint hash = 0);

    static uchar Font_GetRenderIndex();
    static float Font_GetCharacterSizeNormal(GTAChar chr);
    static float Font_GetCharacterSizeDrawing(GTAChar chr, bool use_extra_width);
    static void Font_Render2DPrimitive(const CRect* screen_rect, const CRect* texture_rect, uint color, bool buffered);
    static void Font_PrintChar(float posx, float posy, GTAChar chr, bool buffered);
};
