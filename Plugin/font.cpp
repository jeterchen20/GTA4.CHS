#include "font.h"
#include "game.h"
#include "plugin.h"
#include "table.h"

static const float fChsWidth = 32.0f;
static const float fSpriteWidth = 64.0f;
static const float fSpriteHeight = 80.0f;
static const float fTextureResolution = 4096.0f;
static const float fTextureRowsCount = 51.2f;
static const float fTextureColumnsCount = 64.0f;
static const float fRatio = 4.0f;

static void* pChsFont;

bool CFont::IsNaiveCharacter(GTAChar chr)
{
    return chr < 0x100;
}

void* __fastcall CFont::LoadTextureCB(void* pDictionary, int, uint hash)
{
    void* result = CGame::Dictionary_GetElementByKey(pDictionary, hash);

    pChsFont = CGame::Dictionary_GetElementByKey(pDictionary, CGame::Hash_HashStringFromSeediCase("font_chs"));

    return result;
}

const GTAChar* CFont::SkipAWord(const GTAChar* str)
{
    if (str == nullptr)
    {
        return str;
    }

    auto begin = str;
    auto current = str;

    while (true)
    {
        GTAChar chr = *current;

        if (chr == ' ' || chr == '~' || chr == 0)
        {
            break;
        }

        if (!IsNaiveCharacter(chr))
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

bool compare_token_result;

__declspec(naked) void CFont::Epilog_922054()
{
    static void* ret_addr;

    __asm
    {
        pop ret_addr;

        movzx eax, word ptr[edi];
        cmp eax, '~';
        jz compare_true;

    compare_false:
        mov compare_token_result, 0;
        jmp back;

    compare_true:
        mov compare_token_result, 1;

    back:
        push ret_addr;
        ret;
    }
}

__declspec(naked) void CFont::Prolog_9224A5()
{
    static void* ret_addr;

    __asm
    {
        pop ret_addr;

        mov al, compare_token_result;
        test al, al;
        jnz not_skip;
        call SkipAWord;
        jmp back;

    not_skip:
        mov eax, edi;

    back:
        push ret_addr;
        ret;
    }
}

int CFont::ParseToken(const GTAChar* str, GTAChar* token_string, TokenStruct* token_data)
{
    return injector::cstd<int(const GTAChar*, GTAChar*, TokenStruct*)>::call(CGame::Addresses.pFont_ParseToken, str, token_string, token_data);
}

__declspec(naked) void CFont::GetStringWidthHook()
{
    static void* ret_addr;

    __asm
    {
        pop ret_addr;

        movzx eax, word ptr[esi];
        mov cl, [ebp + 0xC];
        cmp ax, ' ';
        jz space;
        push eax;
        call IsNaiveCharacter;
        add esp, 4;
        test al, al;
        movzx eax, word ptr[esi];
        mov cl, [ebp + 0xC];
        jnz normal;
        jmp chs;

    space:
        add ret_addr, 0x3;
        push ret_addr;
        ret;

    normal:
        add ret_addr, 0xB;
        push ret_addr;
        ret;

    chs:
        test cl, cl; //get all
        jnz normal;
        mov dx, word ptr[esp + 0x12]; //has char1 & has char 2
        test dx, dx;
        mov edx, 807Eh;
        jz normal;
        add ret_addr, 0x22E;
        push ret_addr;
        ret;
    }
}

float CFont::GetCHSCharacterSizeNormal()
{
    uchar index = CGame::Font_GetRenderIndex();

    return ((fChsWidth / *CGame::Addresses.pFont_ResolutionX + CGame::Addresses.pFont_Details[index].fEdgeSize2) * CGame::Addresses.pFont_Details[index].fScaleX);
}

float CFont::GetCharacterSizeNormalDispatch(GTAChar chr)
{
    if (IsNaiveCharacter(chr + 0x20))
    {
        return CGame::Font_GetCharacterSizeNormal(chr);
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

    return (((fChsWidth + extrawidth) / *CGame::Addresses.pFont_ResolutionX + CGame::Addresses.pFont_RenderState->fEdgeSize) * CGame::Addresses.pFont_RenderState->fScaleX);
}

float CFont::GetCharacterSizeDrawingDispatch(GTAChar character, bool use_extra_width)
{
    if (IsNaiveCharacter(character + 0x20))
    {
        return CGame::Font_GetCharacterSizeDrawing(character, use_extra_width);
    }
    else
    {
        return GetCHSCharacterSizeDrawing(use_extra_width);
    }
}

void CFont::PrintCHSChar(float x, float y, GTAChar chr)
{
    CRect screenrect, texturerect;

    if (y < -0.06558f || y > 1.0f)
    {
        return;
    }

    if (-(GetCHSCharacterSizeDrawing(true) / CGame::Addresses.pFont_RenderState->fScaleX) > x || x > 1.0f)
    {
        return;
    }

    auto pos = CCharacterTable::GlobalTable.GetCharPos(chr);

    float sprite_width = fSpriteWidth / fTextureResolution;
    float character_width = (fChsWidth / *CGame::Addresses.pFont_ResolutionX + CGame::Addresses.pFont_RenderState->fEdgeSize) * CGame::Addresses.pFont_RenderState->fScaleX;
    float character_height = CGame::Addresses.pFont_RenderState->fScaleY * 0.06558f;

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

    switch (CGame::Addresses.pFont_RenderState->nFont)
    {
    case 0:
    case 1:
    case 3:
        CGame::Graphics_SetRenderState(pChsFont);
        break;

    default:
        break;
    }

    CGame::Font_Render2DPrimitive(&screenrect, &texturerect, CGame::Addresses.pFont_RenderState->field_18, false);
}

void CFont::PrintCharDispatch(float x, float y, GTAChar chr, bool buffered)
{
    if (CGame::Addresses.pFont_RenderState->TokenType != 0 || IsNaiveCharacter(chr + 0x20))
    {
        CGame::Font_PrintChar(x, y, chr, buffered);
    }
    else
    {
        PrintCHSChar(x, y, chr + 0x20);
    }
}
