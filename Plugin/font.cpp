﻿#include "font.h"
#include "game.h"
#include "plugin.h"
#include "table.h"

namespace Font
{
    static const float fChsWidth = 32.0f;
    static const float fSpriteWidth = 64.0f;
    static const float fSpriteHeight = 80.0f;
    static const float fTextureResolution = 4096.0f;
    static const float fTextureRowsCount = 51.2f;
    static const float fTextureColumnsCount = 64.0f;
    static const float fRatio = 4.0f;

    static void* pChsFont;

    bool IsNaiveCharacter(std::uint16_t character)
    {
        return character < 0x100;
    }

    bool IsNotNaiveCharacter(std::uint16_t character)
    {
        return !IsNaiveCharacter(character);
    }

    void* __fastcall LoadTextureCB(void* pDictionary, int, std::uint32_t hash)
    {
        void* result = Game::Dictionary_GetElementByKey(pDictionary, hash);

        pChsFont = Game::Dictionary_GetElementByKey(pDictionary, Game::Hash_HashStringFromSeediCase("font_chs"));

        return result;
    }

    std::uint16_t* SkipAWord(std::uint16_t* current_text)
    {
        if (current_text == nullptr)
        {
            return current_text;
        }

        std::uint16_t* begin = current_text;
        std::uint16_t* current = current_text;

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

    bool compare_token_result;

    __declspec(naked) void Epilog_922054()
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

    __declspec(naked) void Prolog_9224A5()
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

    __declspec(naked) void GetStringWidthHook()
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

    float GetCHSCharacterSizeNormal()
    {
        std::uint8_t index = Game::Font_GetRenderIndex();

        return ((fChsWidth / *Game::Addresses.pFont_ResolutionX + Game::Addresses.pFont_Details[index].fEdgeSize2) * Game::Addresses.pFont_Details[index].fScaleX);
    }

    float GetCharacterSizeNormalDispatch(std::uint16_t character)
    {
        if (IsNaiveCharacter(character + 0x20))
        {
            return Game::Font_GetCharacterSizeNormal(character);
        }
        else
        {
            return GetCHSCharacterSizeNormal();
        }
    }

    float GetCHSCharacterSizeDrawing(bool useextrawidth)
    {
        float extrawidth = 0.0f;

        if (useextrawidth)
        {
            extrawidth = 1.0f;
        }

        return (((fChsWidth + extrawidth) / *Game::Addresses.pFont_ResolutionX + Game::Addresses.pFont_RenderState->fEdgeSize) * Game::Addresses.pFont_RenderState->fScaleX);
    }

    float GetCharacterSizeDrawingDispatch(std::uint16_t character, bool useextrawidth)
    {
        if (IsNaiveCharacter(character + 0x20))
        {
            return Game::Font_GetCharacterSizeDrawing(character, useextrawidth);
        }
        else
        {
            return GetCHSCharacterSizeDrawing(useextrawidth);
        }
    }

    void PrintCHSChar(float posx, float posy, std::uint16_t character)
    {
        CRageRect screenrect, texturerect;

        if (posy < -0.06558f || posy > 1.0f)
        {
            return;
        }

        if (-(GetCHSCharacterSizeDrawing(true) / Game::Addresses.pFont_RenderState->fScaleX) > posx || posx > 1.0f)
        {
            return;
        }

        auto pos = CCharacterTable::GlobalTable.GetCharPos(character);

        float sprite_width = fSpriteWidth / fTextureResolution;
        float character_width = (fChsWidth / *Game::Addresses.pFont_ResolutionX + Game::Addresses.pFont_RenderState->fEdgeSize) * Game::Addresses.pFont_RenderState->fScaleX;
        float character_height = Game::Addresses.pFont_RenderState->fScaleY * 0.06558f;

        screenrect.fBottomLeftX = posx;
        screenrect.fBottomLeftY = posy + character_height;
        screenrect.fTopRightX = posx + character_width;
        screenrect.fTopRightY = posy;

        texturerect.fTopRightY = (pos.row - 0.045f) * fSpriteHeight / fTextureResolution + 8.0f / fTextureResolution;
        if (texturerect.fTopRightY > 1.0f)
        {
            texturerect.fTopRightY = 1.0f;
        }
        texturerect.fBottomLeftY = (pos.row - 0.045f) * fSpriteHeight / fTextureResolution + 79.0f / fTextureResolution - 0.001f / fRatio + 0.0048f / fRatio;
        texturerect.fBottomLeftX = pos.column / fTextureColumnsCount;
        texturerect.fTopRightX = pos.column / fTextureColumnsCount + sprite_width;

        switch (Game::Addresses.pFont_RenderState->nFont)
        {
        case 0:
        case 1:
        case 3:
            Game::Graphics_SetRenderState(pChsFont);
            break;

        default:
            break;
        }

        Game::Font_Render2DPrimitive(&screenrect, &texturerect, Game::Addresses.pFont_RenderState->field_18, false);
    }

    void PrintCharDispatch(float posx, float posy, std::uint16_t character, bool buffered)
    {
        if (Game::Addresses.pFont_RenderState->TokenType != 0 || IsNaiveCharacter(character + 0x20))
        {
            Game::Font_PrintChar(posx, posy, character, buffered);
        }
        else
        {
            PrintCHSChar(posx, posy, character + 0x20);
        }
    }
}
