#pragma once
#include "../common/stdinc.h"

class CFontDetails
{
public:
    uint field_0;
    float fScaleX;
    float fScaleY;
    float fButtonScaleX;
    int alignment;
    bool bDrawBox;
    bool field_15;
    bool bProportional;
    bool field_17;
    bool bUseUnderscore;
    bool field_19;
    bool field_1A;
    char pad1[1];
    uint BoxColor;
    float fWrapX;
    float fCentreWrapX;
    uchar nFont;
    uchar nExtraWidthIndex;
    char pad2[2];
    float fDropShadowSize;
    uint DropShadowColor;
    float fEdgeSize2;
    float fEdgeSize;
    float fLineHeight;
    float field_40;
    bool field_44;
    char pad3[3];
};

VALIDATE_SIZE(CFontDetails, 0x48);

class CFontInfo
{
public:
    uchar iPropValues[255];
    uchar iTextureMap[255];
    char pad1[2];
    float fUnpropValue;
    float fWidthOfSpaceBetweenChars[5];
    float fJapaneseSubFont1SpaceWidth;
    float fJapaneseSubFont2SpaceWidth;
    int iMainFontStart;
    int iMainFontEnd;
    int iSubFont1Start;
    int iSubFont1End;
    int iSubFont2Start;
    int iSubFont2End;
    int iCommonFontStart;
    int iCommonFontEnd;
    void* pTexture;
    float fTextureWidth;
    float fTextureHeight;
    float fSpriteSize;
    float field_250;
    float field_254;
};
VALIDATE_SIZE(CFontInfo, 0x258);

class CFontRenderState
{
public:
    uint magic;
    float field_4;
    float field_8;
    float fScaleX;
    float fScaleY;
    float field_14;
    uint field_18;
    float field_1C;
    int field_20;
    bool field_24;
    uchar nExtraWidthIndex;
    bool bProportional;
    bool field_27;
    bool bUseUnderscore;
    uchar nFont;
    char pad1[2];
    float fEdgeSize;
    uint TokenType;
    bool field_34;
    char pad2[3];
};
VALIDATE_SIZE(CFontRenderState, 0x38);

struct TokenStruct
{
    int f0[4];
    GTAChar f10[4][32];
    bool f110[4];
};
VALIDATE_SIZE(TokenStruct, 0x114);

struct CFontStringProcess
{
    //0
    virtual ~CFontStringProcess() = default;

    //4
    virtual bool f4(float x, float y, const GTAChar *str_beg, const GTAChar *str_end, float word_width) = 0;

    //8
    virtual bool f8() = 0;
};

class CFont
{
public:
    static bool IsNaiveCharacter(GTAChar chr);

    static float GetCharacterSizeNormalDispatch(GTAChar chr);
    static float GetCHSCharacterSizeNormal();

    static float GetCharacterSizeDrawingDispatch(GTAChar chr, bool use_extra_width);
    static float GetCHSCharacterSizeDrawing(bool use_extra_width);

    static const GTAChar* SkipAWord(const GTAChar* str);
    static const GTAChar* SkipSpaces(const GTAChar* str);

    static void PrintCharDispatch(float x, float y, GTAChar chr, bool buffered);
    static void PrintCHSChar(float x, float y, GTAChar chr);

    static void* __fastcall LoadTextureCB(void*, int, uint);
    static void GetStringWidthHook();
    static void Epilog_922054();
    static void Prolog_9224A5();

    static float GetStringWidth(const GTAChar* str, bool get_all);
    static void ProcessString(float x, float y, const GTAChar* str, CFontStringProcess* processor);
    static int ParseToken(const GTAChar* str, GTAChar* token_string, TokenStruct* token_data); //返回的是一个代表token类型的整数
};
