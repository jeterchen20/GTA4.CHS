#include "font.h"
#include "game.h"
#include "plugin.h"
#include "renderer.h"
#include "logger.h"

CFont FontObject;

static const float fChsWidth = 32.0f;

bool CFont::IsNaiveCharacter(std::uint16_t character)
{
    return (character < 0x100 || character == 0xFFFF);
}

#include "chars.h"
void* __fastcall CFont::LoadTextureHook(void* pDictionary, int, std::uint32_t hash)
{
    //预先渲染文本中出现的字符
    D3DCAPS9 caps;
    (*GameMeta.ppDirect3DDevice9)->GetDeviceCaps(&caps);

    bool pow_of_2_set = ((caps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0);
    bool non_pow_of_2_conditional_set =((caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0);

    if (pow_of_2_set)
        Logger::LogLine(u8"纹理必须是2^n大小");

    if (non_pow_of_2_conditional_set)
        Logger::LogLine(u8"纹理有时候可以不是2^n大小");

    FontObject.renderer.SetD3DDevice(*GameMeta.ppDirect3DDevice9);
    FontObject.renderer.CacheChars(cache_chars);

    return CGame::Dictionary_GetElementByKey(pDictionary, hash);
}

std::uint16_t* CFont::SkipAWord(std::uint16_t* text)
{
    //跳过一个英语单词，或者一个汉字

    if (text == nullptr)
    {
        return text;
    }

    std::uint16_t* begin = text;
    std::uint16_t* current = text;

    while (true)
    {
        std::uint16_t character = *current;

        if (character == ' ' || character == '~' || character == 0)
        {
            break;
        }

        if (!IsNaiveCharacter(character))
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

float CFont::GetCHSCharacterSizeNormal()
{
    std::uint8_t index = CGame::Font_GetRenderIndex();

    return ((fChsWidth / *GameMeta.pFont_ResolutionX + GameMeta.pFont_Details[index].fEdgeSize2) * GameMeta.pFont_Details[index].fScaleX);
}

float CFont::GetCharacterSizeNormalDispatch(std::uint16_t character)
{
    if (IsNaiveCharacter(character + 0x20))
    {
        return injector::cstd<float(std::uint16_t)>::call(GameMeta.pFont_GetCharacterSizeNormal, character);
    }
    else
    {
        return GetCHSCharacterSizeNormal();
    }
}

float CFont::GetCHSCharacterSizeDrawing(bool useextrawidth)
{
    float extrawidth = 0.0f;

    if (useextrawidth)
    {
        extrawidth = 1.0f;
    }

    return (((fChsWidth + extrawidth) / *GameMeta.pFont_ResolutionX + GameMeta.pFont_RenderState->fEdgeSize) * GameMeta.pFont_RenderState->fScaleX);
}

float CFont::GetCharacterSizeDrawingDispatch(std::uint16_t character, bool useextrawidth)
{
    if (IsNaiveCharacter(character + 0x20))
    {
        return CGame::Font_GetCharacterSizeDrawing(character, useextrawidth);
    }
    else
    {
        return GetCHSCharacterSizeDrawing(useextrawidth);
    }
}

void CFont::PrintCHSChar(float posx, float posy, std::uint16_t character)
{
    CRageRect screenrect, texturerect;

    IDirect3DDevice9* dev = *GameMeta.ppDirect3DDevice9;

    if (posy < -0.06558f || posy > 1.0f)
    {
        return;
    }

    if (-(GetCHSCharacterSizeDrawing(true) / GameMeta.pFont_RenderState->fScaleX) > posx || posx > 1.0f)
    {
        return;
    }

    auto& char_texture = FontObject.renderer.LazyGetCharData(character);

    if (char_texture.dx_texture == nullptr)
        return;

    float character_width = (fChsWidth / *GameMeta.pFont_ResolutionX + GameMeta.pFont_RenderState->fEdgeSize) * GameMeta.pFont_RenderState->fScaleX;
    float character_height = GameMeta.pFont_RenderState->fScaleY * 0.06558f;

    screenrect.fBottomLeftX = posx;
    screenrect.fBottomLeftY = posy + character_height;
    screenrect.fTopRightX = posx + character_width;
    screenrect.fTopRightY = posy;

#if 1
    texturerect.fBottomLeftX = 0.0f;
    texturerect.fBottomLeftY = 82.8912f / char_texture.height;
    texturerect.fTopRightY = 4.4f / char_texture.height;
    texturerect.fTopRightX = 1.0f;
#else
    static const float fChsWidth = 32.0f;
    static const float fSpriteWidth = 64.0f;
    static const float fSpriteHeight = 80.0f;
    static const float fTextureResolution = 4096.0f;
    static const float fTextureRowsCount = 51.2f;
    static const float fTextureColumnsCount = 64.0f;
    static const float fTextureRatio = 4.0f;

    //row * 80 - 3.6 + 8 = row * 80 + 4.4
    texturerect.fTopRightY = (pos.row - 0.045f) * fSpriteHeight / fTextureResolution + 8.0f / fTextureResolution;
    if (texturerect.fTopRightY > 1.0f)
    {
        texturerect.fTopRightY = 1.0f;
    }

    //row * 80 - 3.6 + 79.0 - 1.024 + 4.9152 = row * 80 + 82.8912
    texturerect.fBottomLeftY = (pos.row - 0.045f) * fSpriteHeight / fTextureResolution + 79.0f / fTextureResolution - 0.001f / fTextureRatio + 0.0048f / fTextureRatio;

    texturerect.fBottomLeftX = pos.column / fTextureColumnsCount;

    texturerect.fTopRightX = pos.column / fTextureColumnsCount + fSpriteWidth / fTextureResolution;
#endif

    dev->SetTexture(0, char_texture.dx_texture);
    CGame::Font_Render2DPrimitive(&screenrect, &texturerect, GameMeta.pFont_RenderState->field_18, false);

    //TODO: 是否要切换回原来的字库
}

void CFont::PrintCharDispatch(float posx, float posy, std::uint16_t character, bool buffered)
{
    if (GameMeta.pFont_RenderState->TokenType != 0 || IsNaiveCharacter(character + 0x20))
    {
        CGame::Font_PrintChar(posx, posy, character, buffered);
    }
    else
    {
        PrintCHSChar(posx, posy, character + 0x20);
    }
}

const char* CFont::GetTextFileNameHook(int)
{
    return "CHINESE.GXT";
}

__declspec(naked) void CFont::GetStringWidthHook()
{
    static void* retaddr;

    __asm
    {
        pop retaddr; //DDBE45

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
        mov dl, [esp + 0x12]; //has char
        test dl, dl;
        jz normal;
        add retaddr, 0x22E;
        push retaddr;
        ret;
    }
}
