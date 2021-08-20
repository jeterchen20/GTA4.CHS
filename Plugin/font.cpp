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
    if (text == nullptr)
        return text;

    while (*text == ' ')
    {
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

    auto& using_details = CGame::Addresses.pFont_Details[CGame::Font_GetRenderIndex()];
    float current_line_width = 0.0f; //当前行的宽度
    float current_line_width2 = 0.0f; //当前行的宽度2
    float max_line_width = 0.0f; //最宽一行的宽度
    bool had_width = false;
    bool has_continuous_tokens = false;
    bool false_value = false;


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
            int token_id = CGame::Font_ParseToken(++buf_pointer, token_string, &token_data);
            auto token_string_length = std::char_traits<GTAChar>::length(token_string);

            if (token_data.f110 == 0)
            {
                if (token_id < 256 || token_id > 300)
                {
                    //91C666
                    if (token_string_length > 0)
                    {
                        //91C122
                    }

                    if (token_id >= 1000)
                    {
                        //91BFA6
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
                    auto type = reinterpret_cast<uchar*>(&token_data.f110)[type_index];

                    if (type == 1)
                    {
                        //91BF26
                        had_width = true;
                        current_line_width2 = current_line_width;
                    }
                    else if (type == 2)
                    {
                        //91BF53
                        had_width = true;
                        current_line_width2 = current_line_width;
                    }
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
                    current_line_width2 = 0.0f;
                }

                ++buf_pointer;
            }

            ++buf_pointer;

            if (had_width || *buf_pointer == '~')
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
            ++buf_pointer;
        }

        //91C030
        if (false_value && !get_all && using_details.bUseJapaneseSpace)
            break;
    }

    return std::max(current_line_width, max_line_width);
}

float CFont::GetStringWidthRemake(const GTAChar* text, bool get_all)
{
    if (text == nullptr)
        return 0.0f;

    float current_line_width = 0.0f;
    float max_line_width = 0.0f;
    bool had_width = false;
    bool had_continuous_tokens = false;
    auto render_index = CGame::Font_GetRenderIndex();
    auto using_details = CGame::Addresses.pFont_Details[render_index];
    auto single_char_width = CGame::Font_GetCharacterSizeNormal(0);
    auto text_pointer = text;
    GTAChar token_string[64];
    TokenStruct token_data;

    while (true)
    {
        auto chr = *text_pointer;

        if (chr == 0)
            break;

        bool is_chinese_char = IsNaiveCharacter(chr);

        if (chr == ' ' && !get_all)
            break;

        //"english|中文" 的分词
        if (is_chinese_char && had_width && !get_all)
            break;

        if (chr == '~')
        {
            if ((had_width || had_continuous_tokens) && !get_all)
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
                        //91C122
                        had_width = true;
                    }
                }
                else
                {
                    //91C0C3
                    had_width = true;
                }
            }
            else
            {
                for (int token_index = 0; token_index < 4; ++token_index)
                {
                    auto type = reinterpret_cast<uchar*>(&token_data.f110)[token_index];

                    if (type == 1)
                    {
                        //91BF26
                        had_width = true;
                    }
                    else if (type == 2)
                    {
                        //91BF53
                        had_width = true;
                    }
                }
            }

            //91BF9E
            if (token_type >= 1000)
            {
                current_line_width += using_details.fBlipScaleX * dword_1195E98;
                had_width = true;
            }

            while (*text_pointer != '~')
            {
                //91BFE5
                if (*text_pointer == 'n')
                {
                    max_line_width = std::max(max_line_width, current_line_width);
                    current_line_width = 0.0f;
                }

                ++text_pointer;
            }

            ++text_pointer;

            if (*text_pointer == '~')
                had_continuous_tokens = true;
        }
        else
        {
            //单个字符就是一个分词单位
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
    float a74 = 0.0f;
    using_details.field_19 = false;
    float wrap_x = using_details.fWrapX;
    bool is_new_line_token = false;
    float centre_wrap = using_details.fCentreWrapX;
    bool multi_line = processor->MultiLine();
    char temp_color_code = 0;
    float a44 = 0.0f;
    int total_space_count = 0;
    auto temp_details = using_details;
    auto str_beg = text_pointer;
    float line_width_limit = centre_wrap - wrap_x;
    float current_x, draw_x, draw_y;

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

    auto use_japanese_space = using_details.bUseJapaneseSpace;
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

        if (multi_line && !using_details.bUseJapaneseSpace && word_width > line_width_limit)
        {
            //92203B
            text_pointer = str_beg;
            line_width = 0.0f;
            using_details.bUseJapaneseSpace = true;
        }
        else
        {
            if (*text_pointer == '~')
            {
                //922066
                color = -1;
                color_code = 0;
                text_pointer = CGame::Font_ProcessToken(text_pointer, &color, true, &color_code, &key_token_minus_255, &is_new_line_token, token_string, &token_data);
                temp_color_code = color_code;

                if (is_new_line_token)
                {
                    //9220B2
                    line_width += GetCharacterSizeNormalDispatch(0);
                }
            }

            //9220E5
            float val1 = word_width + line_width + a70;

            if ((val1 <= line_width_limit || multi_line) && !is_new_line_token)
            {
                //92248C
                line_width += word_width;
                spaces_width += word_width;
                //TODO: 考虑分词逻辑
                //非Token才SkipWord

                auto skiped_word_pointer = SkipSpaces(text_pointer);
                text_pointer = SkipSpaces(skiped_word_pointer);

                if (!using_details.bUseJapaneseSpace)
                {
                    //922516
                    int space_count = text_pointer - skiped_word_pointer;

                    if (using_details.alignment == 3 && key_token_minus_255 != 0)
                        ++space_count;

                    line_width += static_cast<float>(space_count) * single_space_width;
                    spaces_width += static_cast<float>(space_count) * single_space_width;
                }
                else
                {
                    if (*skiped_word_pointer == ' ')
                    {
                        //9224CE
                        line_width += GetCharacterSizeNormalDispatch(0);
                        spaces_width += GetCharacterSizeNormalDispatch(0);
                    }
                }

                //922589
                if (*text_pointer == 0)
                {
                    //922593
                }


                multi_line = false;
                a44 = line_width;
            }
            else
            {
                //92210A
                if (key_token_minus_255 != 0 || token_string[0] != 0 || token_data.f110 != 0)
                {
                    //回退到token的起始'~'
                    //922120
                    text_pointer -= 2;

                    while (*text_pointer != '~')
                    {
                        --text_pointer;
                    }
                }

                //922141
                multi_line = using_details.bUseJapaneseSpace;
                if (multi_line)
                {
                    //922153
                }

                //9222B3
                if (using_details.alignment == 0)
                {
                    //9222FD
                    draw_x = current_x - (line_width - GetCharacterSizeNormalDispatch(0)) * 0.5f;
                }
                else if (using_details.alignment == 2)
                {
                    //9222C7
                    current_x = centre_wrap;
                    draw_x = centre_wrap - line_width + GetCharacterSizeNormalDispatch(0);
                }
                else
                {
                    if (using_details.alignment != 1)
                    {
                        //92232A
                        if (is_new_line_token || multi_line)
                        {
                            using_details.field_40 = 0.0f;
                        }
                        else
                        {
                            using_details.field_40 = (centre_wrap - a44 - current_x) / static_cast<float>(total_space_count);
                        }
                    }

                    draw_x = current_x;
                    current_x = wrap_x;
                }

                //92237E
                a74 = draw_x;
                if (processor->OnLineFeed(draw_x, y, str_beg, text_pointer, spaces_width))
                {
                    //92238E
                }

                if (!use_japanese_space)
                    using_details.bUseJapaneseSpace = false;

                line_width = 0.0f;
                spaces_width = 0.0f;
                is_new_line_token = false;

                if (temp_color_code != 0)
                {
                    //92240A
                }

                //922473
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

__declspec(naked) void CFont::GetStringWidthHook()
{
    static void* ret_addr;

    __asm
    {
        pop ret_addr; //91BE45

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

        if (text[0] == ' ')
        {
            return SkipWord(text);
        }
        else
        {
            return text;
        }
    }
    else
    {
        processed_token = false;
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
