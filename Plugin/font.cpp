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

void* __fastcall CFont::LoadTextureCB(void* pDictionary, int, uint hash)
{
    void* result = CGame::Dictionary_GetElementByKey(pDictionary, hash);

    pChsFont = CGame::Dictionary_GetElementByKey(pDictionary, CGame::Hash_HashStringFromSeediCase("font_chs"));

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
    static void* ret_addr;

    __asm
    {
        pop ret_addr;

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

float CFont::GetStringWidth(const GTAChar* text, bool get_all)
{
    if (text == nullptr)
        return 0.0f;

    float current_line_width = 0.0f;
    float max_line_width = 0.0f;
    bool had_width = false;
    auto render_index = CGame::Font_GetRenderIndex();
    auto& using_details = CGame::Addresses.pFont_Details[render_index];
    auto single_char_width = CGame::Font_GetCharacterSizeNormal(0);
    auto text_pointer = text;
    GTAChar token_string[64];
    TokenStruct token_data;

    while (true)
    {
        auto chr = *text_pointer;

        if (chr == 0)
            break;

        bool is_chinese_char = IsNativeChar(chr);

        if (chr == ' ' && !get_all)
            break;

        //应对中文在后面的分词
        if (is_chinese_char && had_width && !get_all)
            break;

        if (chr == '~')
        {
            if (had_width && !get_all)
                break;

            token_string[0] = 0;
            ++text_pointer;
            int token_type = CGame::Font_ParseToken(text_pointer, token_string, &token_data);
            auto token_string_length = std::char_traits<GTAChar>::length(token_string);

            if (token_data.f110 == 0)
            {
                if (token_type < 256 || token_type > 300)
                {
                    if (token_string_length > 0)
                    {
                        CGame::Font_AddTokenStringWidth(token_string, &current_line_width, render_index);
                        had_width = true;
                    }
                }
                else
                {
                    current_line_width += CGame::Addresses.pFont_ButtonWidths[token_type - 0x100 + 1] * using_details.fBlipScaleX;

                    if (using_details.bIgnoreWidthLimit)
                        current_line_width *= 2.0f; //行首的Button2倍宽度？

                    had_width = true;
                }
            }
            else
            {
                for (int token_index = 0; token_index < 4; ++token_index)
                {
                    auto type = token_data.a110[token_index];

                    if (type == 1)
                    {
                        current_line_width += CGame::Addresses.pFont_ButtonWidths[type - 0x100 + 1] * using_details.fBlipScaleX;
                        had_width = true;
                    }
                    else if (type == 2)
                    {
                        CGame::Font_AddTokenStringWidth(token_data.f10[token_index], &current_line_width, render_index);
                        had_width = true;
                    }
                }
            }

            if (token_type >= 1000)
            {
                current_line_width += using_details.fBlipScaleX * *CGame::Addresses.pFont_BlipWidth;
                had_width = true;
            }

            while (*text_pointer != '~')
            {
                if (*text_pointer == 'n')
                {
                    max_line_width = std::max(max_line_width, current_line_width);
                    current_line_width = 0.0f;
                }

                ++text_pointer;
            }

            ++text_pointer;

            //在非get_all的情况下，处理一个Token后立即跳出
            if (had_width && !get_all)
                break;
        }
        else
        {
            auto chr_width = GetCharacterSizeNormalDispatch(chr - 0x20);
            current_line_width += chr_width;

            if (chr != ' ')
                had_width = true;

            ++text_pointer;

            //在非get_all的情况下，处理一个中文字符后立即跳出
            if (is_chinese_char && !get_all)
                break;
        }
    }

    return std::max(current_line_width, max_line_width);
}

void CFont::ProcessString(float x, float y, const GTAChar* text, CFontStringProcess* processor)
{
    auto text_pointer = text;

    if (text_pointer == nullptr)
        return;

    auto& using_details = CGame::Addresses.pFont_Details[CGame::Font_GetRenderIndex()];
    float single_space_width = GetCharacterSizeNormalDispatch(0);
    float line_width = 0.0f;
    float spaces_width = 0.0f;
    float a70 = 0.0f;
    float xstart = 0.0f;
    using_details.bUseColor = false;
    float wrap_x = using_details.fWrapX;
    bool is_new_line_token = false;
    float centre_wrap = using_details.fCentreWrapX;
    bool multi_line = processor->Func8();
    char temp_color_code = 0;
    float a44 = 0.0f;
    int total_space_count = 0;
    auto temp_details = using_details;
    auto str_beg = text_pointer;
    float line_width_limit = centre_wrap - wrap_x;
    float current_x;
    bool had_width = false;

    GTAChar chopped_string[1200];

    switch (using_details.alignment)
    {
    case 1:
    case 2:
    case 3:
    {
        current_x = x;
        a70 = x - wrap_x;
        if (a70 < 0.0f)
            a70 = 0.0f;

        break;
    }

    default:
    {
        auto diff1 = centre_wrap - x;
        auto diff2 = x - wrap_x;
        current_x = x;

        if (diff2 < diff1)
            diff1 = diff2;

        if (diff1 >= 0.0f)
        {
            wrap_x = x - diff1;
            centre_wrap = diff1 + x;
            line_width_limit = diff1 * 2.0f;
        }
        else
        {
            current_x = (centre_wrap + wrap_x) * 0.5f;
        }

        break;
    }
    }

    auto use_japanese_space = using_details.bIgnoreWidthLimit;
    GTAChar token_string[64];
    std::fill(std::begin(token_string), std::end(token_string), 0);
    TokenStruct token_data;

    if (*text_pointer == 0)
        return;

    int color;
    char color_code;
    int key_token_minus_255;

    while (true)
    {
        key_token_minus_255 = 0;

        float word_width = GetStringWidth(text_pointer, false);
        token_string[0] = 0;
        token_data.f110 = 0;

        if (multi_line && !using_details.bIgnoreWidthLimit && word_width > line_width_limit)
        {
            text_pointer = str_beg;
            line_width = 0.0f;
            using_details.bIgnoreWidthLimit = true;
        }
        else
        {
            bool processed_token = false;

            if (*text_pointer == '~')
            {
                color = -1;
                color_code = 0;
                text_pointer = CGame::Font_ProcessToken(text_pointer, &color, true, &color_code, &key_token_minus_255, &is_new_line_token, token_string, &token_data);
                processed_token = true;
                temp_color_code = color_code;

                if (is_new_line_token)
                {
                    line_width += single_space_width;
                }
            }

            float val1 = word_width + line_width + a70;

            if ((val1 <= line_width_limit || multi_line) && !is_new_line_token)
            {
                line_width += word_width;
                spaces_width += word_width;

                int space_count = 0;

                auto skiped_word_pointer = text_pointer;
                if (!processed_token)
                {
                    skiped_word_pointer = SkipWord(text_pointer);
                }

                text_pointer = SkipSpaces(skiped_word_pointer);


                if (!using_details.bIgnoreWidthLimit)
                {
                    space_count = text_pointer - skiped_word_pointer;

                    if (using_details.alignment == 3 && key_token_minus_255 != 0)
                        ++space_count;

                    line_width += static_cast<float>(space_count) * single_space_width;
                    spaces_width += static_cast<float>(space_count) * single_space_width;
                }
                else
                {
                    if (*skiped_word_pointer == ' ')
                    {
                        line_width += single_space_width;
                        spaces_width += single_space_width;
                    }
                }

                if (*text_pointer == 0)
                {
                    switch (using_details.alignment)
                    {
                    case 0:
                    {
                        xstart = current_x - line_width * 0.5f;
                        break;
                    }

                    case 1:
                    {
                        xstart = current_x;
                        break;
                    }

                    case 2:
                    {
                        xstart = centre_wrap - line_width;
                        break;
                    }

                    case 3:
                    {
                        using_details.fExtraSpaceWidth = 0.0f;
                        xstart = current_x;
                        break;
                    }

                    default:
                    {
                        break;
                    }
                    }

                    processor->ProcessStringPart(xstart, y, str_beg, text_pointer, spaces_width);
                }

                if (!multi_line)
                    total_space_count += space_count;

                multi_line = false;
                a44 = line_width;
            }
            else
            {
                if (key_token_minus_255 != 0 || token_string[0] != 0 || token_data.f110 != 0)
                {
                    text_pointer -= 2;

                    while (*text_pointer != '~')
                    {
                        --text_pointer;
                    }
                }

                multi_line = using_details.bIgnoreWidthLimit;
                if (multi_line)
                {
                    //TODO: 922153  
                }

                if (using_details.alignment == 0)
                {
                    xstart = current_x - (line_width - single_space_width) * 0.5f;
                }
                else if (using_details.alignment == 2)
                {
                    current_x = centre_wrap;
                    xstart = centre_wrap - (line_width - single_space_width);
                }
                else
                {
                    if (using_details.alignment != 1)
                    {
                        if (is_new_line_token || multi_line)
                        {
                            using_details.fExtraSpaceWidth = 0.0f;
                        }
                        else
                        {
                            using_details.fExtraSpaceWidth = (centre_wrap - a44 - current_x) / static_cast<float>(total_space_count);
                        }
                    }

                    xstart = current_x;
                    current_x = wrap_x;
                }

                if (processor->ProcessStringPart(xstart, y, str_beg, text_pointer, spaces_width))
                {
                    y += CGame::Font_GetActualLineHeight();
                }

                if (!use_japanese_space)
                    using_details.bIgnoreWidthLimit = false;

                line_width = 0.0f;
                spaces_width = 0.0f;
                is_new_line_token = false;

                if (temp_color_code != 0)
                {
                    chopped_string[0] = '~';
                    chopped_string[1] = static_cast<uchar>(temp_color_code);
                    chopped_string[2] = '~';

                    auto rest_length = std::char_traits<GTAChar>::length(text_pointer);
                    if (rest_length + 3 < 1200)
                    {
                        std::char_traits<GTAChar>::copy(&chopped_string[3], text_pointer, rest_length);
                        chopped_string[rest_length + 3] = 0;
                        text_pointer = chopped_string;
                    }

                    temp_color_code = 0;
                }

                str_beg = text_pointer;
                total_space_count = 0;
                a70 = 0.0f;
                multi_line = true;
                a44 = 0.0f;
            }
        }

        if (*text_pointer == 0)
            break;
    }

    using_details = temp_details;
}

void CFont::ProcessStringRemake(float x, float y, const GTAChar* text, CFontStringProcess* processor)
{
    auto text_pointer = text;

    if (text_pointer == nullptr)
        return;

    auto& using_details = CGame::Addresses.pFont_Details[CGame::Font_GetRenderIndex()];
    float single_space_width = GetCharacterSizeNormalDispatch(0);
    float line_width = 0.0f;
    float spaces_width = 0.0f;
    float a70 = 0.0f;
    float xstart = 0.0f;
    using_details.bUseColor = false;
    float wrap_x = using_details.fWrapX;
    bool is_new_line_token = false;
    float centre_wrap = using_details.fCentreWrapX;
    bool multi_line = processor->Func8();
    char temp_color_code = 0;
    float a44 = 0.0f;
    int total_space_count = 0;
    auto temp_details = using_details;
    auto str_beg = text_pointer;
    float line_width_limit = centre_wrap - wrap_x;
    float current_x;
    bool had_width = false;

    GTAChar chopped_string[1200];

    switch (using_details.alignment)
    {
    case 1:
    case 2:
    case 3:
    {
        current_x = x;
        a70 = x - wrap_x;
        if (a70 < 0.0f)
            a70 = 0.0f;

        break;
    }

    default:
    {
        auto diff1 = centre_wrap - x;
        auto diff2 = x - wrap_x;
        current_x = x;

        if (diff2 < diff1)
            diff1 = diff2;

        if (diff1 >= 0.0f)
        {
            wrap_x = x - diff1;
            centre_wrap = diff1 + x;
            line_width_limit = diff1 * 2.0f;
        }
        else
        {
            current_x = (centre_wrap + wrap_x) * 0.5f;
        }

        break;
    }
    }

    auto use_japanese_space = using_details.bIgnoreWidthLimit;
    GTAChar token_string[64];
    std::fill(std::begin(token_string), std::end(token_string), 0);
    TokenStruct token_data;

    if (*text_pointer == 0)
        return;

    int color;
    char color_code;
    int key_token_minus_255;

    while (true)
    {
        key_token_minus_255 = 0;

        float word_width = GetStringWidth(text_pointer, false);
        token_string[0] = 0;
        token_data.f110 = 0;

        if (multi_line && !using_details.bIgnoreWidthLimit && word_width > line_width_limit)
        {
            text_pointer = str_beg;
            line_width = 0.0f;
            using_details.bIgnoreWidthLimit = true;
        }
        else
        {
            bool processed_token = false;

            if (*text_pointer == '~')
            {
                color = -1;
                color_code = 0;
                text_pointer = CGame::Font_ProcessToken(text_pointer, &color, true, &color_code, &key_token_minus_255, &is_new_line_token, token_string, &token_data);
                processed_token = true;
                temp_color_code = color_code;

                if (is_new_line_token)
                {
                    line_width += single_space_width;
                }
            }

            float val1 = word_width + line_width + a70;

            if ((val1 <= line_width_limit || multi_line) && !is_new_line_token)
            {
                line_width += word_width;
                spaces_width += word_width;

                int space_count = 0;

                auto skiped_word_pointer = text_pointer;
                if (!processed_token)
                {
                    skiped_word_pointer = SkipWord(text_pointer);
                }

                text_pointer = SkipSpaces(skiped_word_pointer);


                if (!using_details.bIgnoreWidthLimit)
                {
                    space_count = text_pointer - skiped_word_pointer;

                    if (using_details.alignment == 3 && key_token_minus_255 != 0)
                        ++space_count;

                    line_width += static_cast<float>(space_count) * single_space_width;
                    spaces_width += static_cast<float>(space_count) * single_space_width;
                }
                else
                {
                    if (*skiped_word_pointer == ' ')
                    {
                        line_width += single_space_width;
                        spaces_width += single_space_width;
                    }
                }

                if (*text_pointer == 0)
                {
                    switch (using_details.alignment)
                    {
                    case 0:
                    {
                        xstart = current_x - line_width * 0.5f;
                        break;
                    }

                    case 1:
                    {
                        xstart = current_x;
                        break;
                    }

                    case 2:
                    {
                        xstart = centre_wrap - line_width;
                        break;
                    }

                    case 3:
                    {
                        using_details.fExtraSpaceWidth = 0.0f;
                        xstart = current_x;
                        break;
                    }

                    default:
                    {
                        break;
                    }
                    }

                    processor->ProcessStringPart(xstart, y, str_beg, text_pointer, spaces_width);
                }

                if (!multi_line)
                    total_space_count += space_count;

                multi_line = false;
                a44 = line_width;
            }
            else
            {
                if (key_token_minus_255 != 0 || token_string[0] != 0 || token_data.f110 != 0)
                {
                    text_pointer -= 2;

                    while (*text_pointer != '~')
                    {
                        --text_pointer;
                    }
                }

                multi_line = using_details.bIgnoreWidthLimit;
                if (multi_line)
                {
                    //TODO: 922153  
                }

                if (using_details.alignment == 0)
                {
                    xstart = current_x - (line_width - single_space_width) * 0.5f;
                }
                else if (using_details.alignment == 2)
                {
                    current_x = centre_wrap;
                    xstart = centre_wrap - (line_width - single_space_width);
                }
                else
                {
                    if (using_details.alignment != 1)
                    {
                        if (is_new_line_token || multi_line)
                        {
                            using_details.fExtraSpaceWidth = 0.0f;
                        }
                        else
                        {
                            using_details.fExtraSpaceWidth = (centre_wrap - a44 - current_x) / static_cast<float>(total_space_count);
                        }
                    }

                    xstart = current_x;
                    current_x = wrap_x;
                }

                if (processor->ProcessStringPart(xstart, y, str_beg, text_pointer, spaces_width))
                {
                    y += CGame::Font_GetActualLineHeight();
                }

                if (!use_japanese_space)
                    using_details.bIgnoreWidthLimit = false;

                line_width = 0.0f;
                spaces_width = 0.0f;
                is_new_line_token = false;

                if (temp_color_code != 0)
                {
                    chopped_string[0] = '~';
                    chopped_string[1] = static_cast<uchar>(temp_color_code);
                    chopped_string[2] = '~';

                    auto rest_length = std::char_traits<GTAChar>::length(text_pointer);
                    if (rest_length + 3 < 1200)
                    {
                        std::char_traits<GTAChar>::copy(&chopped_string[3], text_pointer, rest_length);
                        chopped_string[rest_length + 3] = 0;
                        text_pointer = chopped_string;
                    }

                    temp_color_code = 0;
                }

                str_beg = text_pointer;
                total_space_count = 0;
                a70 = 0.0f;
                multi_line = true;
                a44 = 0.0f;
            }
        }

        if (*text_pointer == 0)
            break;
    }

    using_details = temp_details;
}

float CFont::GetMaxWordWidth(const GTAChar* text)
{
    if (text == nullptr)
        return 0.0f;

    float max_word_width = 0.0f;

    while (*text != 0)
    {
        float word_width = GetStringWidth(text, false);

        if (word_width > max_word_width)
            max_word_width = word_width;

        text = SkipSpaces(SkipWord(text));
    }

    return max_word_width;
}

float CFont::GetCHSCharacterSizeNormal()
{
    uchar index = CGame::Font_GetRenderIndex();

    return ((fChsWidth / *CGame::Addresses.pFont_ResolutionX + CGame::Addresses.pFont_Details[index].fEdgeSize2) * CGame::Addresses.pFont_Details[index].fScaleX);
}

float CFont::GetCharacterSizeNormalDispatch(GTAChar chr)
{
    if (IsNativeChar(chr + 0x20))
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
    if (IsNativeChar(chr + 0x20))
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
    if (CGame::Addresses.pFont_RenderState->TokenType != 0 || IsNativeChar(chr + 0x20))
    {
        CGame::Font_PrintChar(x, y, chr, buffered);
    }
    else
    {
        PrintCHSChar(x, y, chr + 0x20);
    }
}
