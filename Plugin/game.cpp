#include "game.h"

StockAddresses CGame::Addresses;

void CGame::Graphics_SetRenderState(void* texture, int arg4)
{
    injector::cstd<void(void*, int)>::call(Addresses.pGraphics_SetRenderState, texture, arg4);
}

void* CGame::Dictionary_GetElementByKey(void* dictionary, uint hash)
{
    return injector::thiscall<void* (void*, uint)>::call(Addresses.pDictionary_GetElementByKey, dictionary, hash);
}

uint CGame::Hash_HashStringFromSeediCase(const char* str, uint hash)
{
    return injector::cstd<uint(const char*, uint)>::call(Addresses.pHash_HashStringFromSeediCase, str, hash);
}

uchar CGame::Font_GetRenderIndex()
{
    return injector::cstd<uchar()>::call(Addresses.pFont_GetRenderIndex);
}

float CGame::Font_GetCharacterSizeNormal(GTAChar chr)
{
    return injector::cstd<float(GTAChar)>::call(Addresses.pFont_GetCharacterSizeNormal, chr);
}

float CGame::Font_GetCharacterSizeDrawing(GTAChar chr, bool use_extra_width)
{
    return injector::cstd<float(GTAChar, bool)>::call(Addresses.pFont_GetCharacterSizeDrawing, chr, use_extra_width);
}

void CGame::Font_Render2DPrimitive(const CRect* screen_rect, const CRect* texture_rect, uint color, bool buffered)
{
    injector::cstd<void(const CRect*, const CRect*, uint, bool)>::call(Addresses.pFont_Render2DPrimitive, screen_rect, texture_rect, color, buffered);
}

void CGame::Font_PrintChar(float posx, float posy, GTAChar chr, bool buffered)
{
    injector::cstd<void(float, float, GTAChar, bool)>::call(Addresses.pFont_PrintChar, posx, posy, chr, buffered);
}

int CGame::Font_ParseToken(const GTAChar* text, GTAChar* text_to_show, TokenStruct* token_data)
{
    return injector::cstd<int(const GTAChar*, GTAChar*, TokenStruct*)>::call(
        Addresses.pFont_ParseToken, text, text_to_show, token_data);
}

const GTAChar* CGame::Font_ProcessToken(const GTAChar* text, int* color, bool get_color_code_only, char* color_code,
    int* key_number, bool* is_new_line_token, GTAChar* text_to_show, TokenStruct* token_data)
{
    return injector::cstd<const GTAChar* (const GTAChar*, int*, bool, char*, int*, bool*, GTAChar*, TokenStruct*)>::call(
        Addresses.pFont_ProcessToken, text, color, get_color_code_only, color_code, key_number, is_new_line_token,
        text_to_show, token_data);
}
