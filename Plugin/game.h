#pragma once
#include "../common/stdinc.h"
#include "font.h"

struct StockAddresses
{
    IDirect3D9** ppDirect3D9;
    IDirect3DDevice9** ppDirect3DDevice9;

    void* pGraphics_SetRenderState;
    void* pDictionary_GetElementByKey;
    void* pHash_HashStringFromSeediCase;

    CFontInfo* pFont_Datas;
    CFontDetails* pFont_Details;
    CFontRenderState* pFont_RenderState;

    float* pFont_BlipWidth;
    float* pFont_ResolutionX;
    float *pFont_ButtonWidths;

    void* pFont_GetRenderIndex;
    void* pFont_PrintChar;
    void* pFont_GetCharacterSizeNormal;
    void* pFont_GetCharacterSizeDrawing;
    void* pFont_Render2DPrimitive;
    void* pFont_AddTokenStringWidth;
    void* pFont_ParseToken;
    void* pFont_ProcessToken;
    void* pFont_GetActualLineHeight;
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

    //ParseToken逻辑
    //1000+ BLIP
    //256-300 按键类
    //全部strcmp都不通过则当成单字符token，返回此字符
    static int Font_ParseToken(const GTAChar* text, GTAChar* text_to_show, TokenStruct* token_data);
    static const GTAChar* Font_ProcessToken(const GTAChar* text, int* color, bool get_color, char* color_code, int* key_number, bool* is_new_line_token, GTAChar* text_to_show, TokenStruct* token_data);
    static void Font_AddTokenStringWidth(const GTAChar* text, float* width, int render_index);
    static float Font_GetActualLineHeight();
};
