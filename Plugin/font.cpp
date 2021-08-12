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

const GTAChar* CFont::SkipWord(const GTAChar* str)
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

const GTAChar* CFont::SkipSpaces(const GTAChar* text)
{
    while (true)
    {
        if (text == nullptr || *text != ' ')
            break;

        ++text;
    }

    return text;
}

float CFont::GetStringWidth(const GTAChar* text, bool get_all)
{
    GTAChar str_buf[2048];
    TokenStruct token_data;
    GTAChar token_string[64];

    if (text == nullptr)
        return 0.0f;

    float current_line_width = 0.0f; //当前行的宽度
    float max_line_width = 0.0f; //最宽一行的宽度
    bool had_width = false;
    bool has_continuous_tokens = false;
    bool had_drawable_token = false;
    auto render_index = CGame::Font_GetRenderIndex();

    auto text_length = std::char_traits<GTAChar>::length(text);
    if (text_length > 2047)
        throw std::out_of_range("String too long.");

    std::char_traits<GTAChar>::copy(str_buf, text, text_length);
    str_buf[text_length] = 0;

    auto buf_pointer = str_buf;

    while (true)
    {
        auto chr = *buf_pointer;

        if (chr == 0 || (chr == ' ' && !get_all))
            break;

        if (chr == '~')
        {
            if (!get_all && (has_continuous_tokens || had_width))
                break;

            token_string[0] = 0;
            int token_id = CGame::Font_ParseToken(buf_pointer, token_string, &token_data);
            auto token_string_length = std::char_traits<GTAChar>::length(token_string);

            if (token_data.f110 == 0)
            {
                if (token_id > 300)
                {
                    if (token_string_length > 0)
                    {
                        //91C122
                    }
                }
                else
                {
                    //91C0C3
                }
            }
            else
            {
                //91BF11
                for (int type_index = 0; type_index < 4; ++type_index)
                {
                    auto type = reinterpret_cast<uchar *>(&token_data.f110)[type_index];

                    if (type == 1)
                    {
                        //91BF26
                    }
                    else if (type == 2)
                    {
                        //91BF53
                    }

                    had_drawable_token = true;
                }
            }

            if (token_id >= 1000)
            {
                //91BFA6
            }

            while (*buf_pointer != '~')
            {
                //91BFE0
                if (*buf_pointer == 'n')
                {
                    if (current_line_width > max_line_width)
                        max_line_width = current_line_width;

                    current_line_width = 0.0f;
                }

                ++buf_pointer;
            }

            ++buf_pointer;

            if (had_drawable_token || *buf_pointer == '~')
                has_continuous_tokens = true;
        }
        else
        {
            //TODO: 增加中文分词判断
            if (!get_all && chr == ' ' && has_continuous_tokens)
            {
                break;
            }

            current_line_width += GetCharacterSizeNormalDispatch(chr - 0x20);
        }

        ++buf_pointer;
    }

    return std::max(current_line_width, max_line_width);
}

void CFont::ProcessString(float x, float y, const GTAChar* text, CFontStringProcess* processor)
{
    bool multi_line = processor->MultiLine();
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

static bool processed_token = false;

const GTAChar* CFont::SkipWord_Prolog(const GTAChar* text)
{
    if (processed_token)
    {
        processed_token = false;
        return text;
    }
    else
    {
        return SkipWord(text);
    }
}

const GTAChar* CFont::ProcessToken_Prolog(const GTAChar* text, int* color, bool get_color_code_only, char* color_code,
    int* key_number, bool* is_new_line_token, GTAChar* text_to_show,
    TokenStruct* token_data)
{
    processed_token = true;

    return CGame::Font_ProcessToken(text, color, get_color_code_only, color_code, key_number, is_new_line_token, text_to_show,
        token_data);
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

float CFont::GetCharacterSizeDrawingDispatch(GTAChar chr, bool use_extra_width)
{
    if (IsNaiveCharacter(chr + 0x20))
    {
        return CGame::Font_GetCharacterSizeDrawing(chr, use_extra_width);
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

    auto pos = GlobalTable.GetCharPos(chr);

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
