#include "game.h"

namespace Game
{
    GameAddresses Addresses;

    void Graphics_SetRenderState(void* texture, int arg4)
    {
        injector::cstd<void(void*, int)>::call(Addresses.pGraphics_SetRenderState, texture, arg4);
    }

    void* Dictionary_GetElementByKey(void* dictionary, uint hash)
    {
        return injector::thiscall<void* (void*, uint)>::call(Addresses.pDictionary_GetElementByKey, dictionary, hash);
    }

    uint Hash_HashStringFromSeediCase(const char* str, uint hash)
    {
        return injector::cstd<uint(const char*, uint)>::call(Addresses.pHash_HashStringFromSeediCase, str, hash);
    }

    uchar Font_GetRenderIndex()
    {
        return injector::cstd<uchar()>::call(Addresses.pFont_GetRenderIndex);
    }

    float Font_GetCharacterSizeNormal(GTAChar character)
    {
        return injector::cstd<float(GTAChar)>::call(Addresses.pFont_GetCharacterSizeNormal, character);
    }

    float Font_GetCharacterSizeDrawing(GTAChar character, bool use_extra_width)
    {
        return injector::cstd<float(GTAChar, bool)>::call(Addresses.pFont_GetCharacterSizeDrawing, character, use_extra_width);
    }

    void Font_Render2DPrimitive(const CRect* screen_rect, const CRect* texture_rect, uint color, bool buffered)
    {
        injector::cstd<void(const CRect*, const CRect*, uint, bool)>::call(Addresses.pFont_Render2DPrimitive, screen_rect, texture_rect, color, buffered);
    }

    void Font_PrintChar(float posx, float posy, GTAChar character, bool buffered)
    {
        injector::cstd<void(float, float, GTAChar, bool)>::call(Addresses.pFont_PrintChar, posx, posy, character, buffered);
    }
}
