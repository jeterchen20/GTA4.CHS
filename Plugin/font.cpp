﻿#include "font.h"
#include "plugin.h"
#include "table.h"

static const float fChsWidth = 32.0f;
static const float fSpriteWidth = 64.0f;
static const float fSpriteHeight = 80.0f;
static const float fTextureResolution = 4096.0f;
static const float fTextureRowsCount = 51.2f;
static const float fTextureColumnsCount = 64.0f;
static const float fRatio = 4.0f;

static void* pChsTextureDictionary;
static void* pChsTexture;

void* __fastcall CFont::LoadTextureCB(void* pDictionary, int, uint hash)
{
    void* result = plugin.game.Dictionary_GetElementByKey(pDictionary, hash);

    pChsTexture = plugin.game.Dictionary_GetElementByKey(pDictionary, plugin.game.Hash_HashStringFromSeediCase("font_chs"));

    return result;
}

const GTAChar* CFont::SkipWord(const GTAChar* text)
{
    if (text == nullptr)
    {
        return text;
    }

    auto begin = text;
    auto current = text;

    while (true)
    {
        GTAChar chr = *current;

        if (chr == ' ' || chr == '~' || chr == 0)
        {
            break;
        }

        if (!IsNativeChar(chr))
        {
            if (current == begin)
            {
                ++current;
            }

            break;
        }
        else
        {
            ++current;
        }
    }

    return current;
}

const GTAChar* CFont::SkipSpaces(const GTAChar* text)
{
    if (text == nullptr)
        return text;

    while (*text == ' ')
    {
        ++text;
    }

    return text;
}

__declspec(naked) void CFont::GetStringWidthHook()
{
    static void* retaddr;

    __asm
    {
        pop retaddr; //91BE45

        movzx eax, word ptr[esi];
        mov cl, [ebp + 0xC];
        cmp ax, ' ';
        jz space;
        push eax;
        call IsNativeChar;
        add esp, 4;
        test al, al;
        movzx eax, word ptr[esi];
        mov cl, [ebp + 0xC];
        jnz normal;
        jmp chs;

    space:
        add retaddr, 0x3;
        push retaddr;
        ret;

    normal:
        add retaddr, 0xB;
        push retaddr;
        ret;

    chs:
        test cl, cl; //get all
        jnz normal;
#if 1
        mov dl, [esp + 0x12]; //has char
        test dl, dl;
#else
        mov dx, [esp + 0x12]; //has char1 & has char 2
        test dx, dx;
#endif
        mov edx, 0x807E;
        jz normal;
        add retaddr, 0x22E;
        push retaddr;
        ret;
    }
}

float CFont::GetCHSCharacterSizeNormal()
{
    uchar index = plugin.game.Font_GetRenderIndex();


    return ((fChsWidth / *plugin.game.game_addr.pFont_ResolutionX + plugin.game.game_addr.pFont_Details[index].fEdgeSize2) * plugin.game.game_addr.pFont_Details[index].fScaleX);
}

float CFont::GetCharacterSizeNormalDispatch(GTAChar chr)
{
    if (IsNativeChar(chr + 0x20))
    {
        return plugin.game.Font_GetCharacterSizeNormal(chr);
    }
    else
    {
        return GetCHSCharacterSizeNormal();
    }
}

float CFont::GetCHSCharacterSizeDrawing(bool use_extra_width)
{
    float extrawidth = 0.0f;

    if (use_extra_width)
    {
        extrawidth = 1.0f;
    }

    return (((fChsWidth + extrawidth) / *plugin.game.game_addr.pFont_ResolutionX + plugin.game.game_addr.pFont_RenderState->fEdgeSize) * plugin.game.game_addr.pFont_RenderState->fScaleX);
}

float CFont::GetCharacterSizeDrawingDispatch(GTAChar chr, bool use_extra_width)
{
    if (IsNativeChar(chr + 0x20))
    {
        return plugin.game.Font_GetCharacterSizeDrawing(chr, use_extra_width);
    }
    else
    {
        return GetCHSCharacterSizeDrawing(use_extra_width);
    }
}

void CFont::PrintCHSChar(float x, float y, GTAChar chr)
{
    CRect screenrect, texturerect;

    //跳过全角空格
    if (chr == 0x3000)
        return;

    if (y < -0.06558f || y > 1.0f)
    {
        return;
    }

    if (-(GetCHSCharacterSizeDrawing(true) / plugin.game.game_addr.pFont_RenderState->fScaleX) > x || x > 1.0f)
    {
        return;
    }

    auto pos = plugin.game.table.GetCharPos(chr);

    float sprite_width = fSpriteWidth / fTextureResolution;
    float character_width = (fChsWidth / *plugin.game.game_addr.pFont_ResolutionX + plugin.game.game_addr.pFont_RenderState->fEdgeSize) * plugin.game.game_addr.pFont_RenderState->fScaleX;
    float character_height = plugin.game.game_addr.pFont_RenderState->fScaleY * 0.06558f;

    screenrect.fBottomLeftX = x;
    screenrect.fBottomLeftY = y + character_height;
    screenrect.fTopRightX = x + character_width;
    screenrect.fTopRightY = y;

    texturerect.fTopRightY = (pos.row - 0.045f) * fSpriteHeight / fTextureResolution + 8.0f / fTextureResolution;
    if (texturerect.fTopRightY > 1.0f)
    {
        texturerect.fTopRightY = 1.0f;
    }
    texturerect.fBottomLeftY = (pos.row - 0.045f) * fSpriteHeight / fTextureResolution + 79.0f / fTextureResolution - 0.001f / fRatio + 0.0048f / fRatio;
    texturerect.fBottomLeftX = pos.column / fTextureColumnsCount;
    texturerect.fTopRightX = pos.column / fTextureColumnsCount + sprite_width;

    switch (plugin.game.game_addr.pFont_RenderState->nFont)
    {
    case 0:
    case 1:
    case 3:
        plugin.game.Graphics_SetRenderState(pChsTexture);
        break;

    default:
        break;
    }

    plugin.game.Font_Render2DPrimitive(&screenrect, &texturerect, plugin.game.game_addr.pFont_RenderState->field_18, false);
}

void CFont::PrintCharDispatch(float x, float y, GTAChar chr, bool buffered)
{
    if (plugin.game.game_addr.pFont_RenderState->TokenType != 0 || IsNativeChar(chr + 0x20))
    {
        plugin.game.Font_PrintChar(x, y, chr, buffered);
    }
    else
    {
        PrintCHSChar(x, y, chr + 0x20);
    }
}
