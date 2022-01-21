#pragma once
#include "../common/stdinc.h"

struct IVTextTableSorting
{
    //保证'MAIN'是第一个
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        //确保两个'MAIN'比较结果是false
        if (rhs == "MAIN")
        {
            return false;
        }
        else if (lhs == "MAIN")
        {
            return true;
        }
        else
        {
            return lhs < rhs;
        }
    }
};

struct GXTHeader
{
    std::uint16_t Version; //4
    std::uint16_t CharBits; //8 or 16, 16 required
};

struct TableEntry
{
    char Name[8];
    std::int32_t Offset;
};

struct TableBlock
{
    char TABL[4];
    std::int32_t Size;
    //TableEntry Entries[Size / 12];
};

struct KeyEntry
{
    std::int32_t Offset;
    std::uint32_t Hash;
};

struct KeyBlockMAIN
{
    char TKEY[4];
    std::int32_t Size;
};

struct KeyBlockOthers
{
    char Name[8];
    KeyBlockMAIN Body;
};

struct DataBlock
{
    char TDAT[4];
    std::int32_t Size;
    //std::uint16_t Entries[Size / 2];
};

class IVText
{
public:
    static constexpr unsigned Texture_CharsPerLine = 64;

    typedef std::filesystem::path PathType; //路径类型
    typedef GTAChar CharType; //宽字符(GXT)类型
    typedef std::uint32_t HashType; //GXT key类型
    typedef tiny_utf8::utf8_string u8StringType; //转码用
    typedef std::vector<GTAChar> wStringType; //GXT字符串类型(手动补0)

    struct TextEntry
    {
        HashType hash;
        u8StringType text;
    };

    //不带参数，text目录转gxt
    void Process0Arg();

    //文件, 目录，gxt转文本
    //目录, 目录，文本转gxt
    void Process2Args(const PathType& arg1, const PathType& arg2);

private:
    //读取流时跳过UTF8签名
    static void SkipUTF8Signature(std::ifstream& stream);

    static bool IsNativeCharacter(char32_t character);

    //收集文本中需要添加到字库的字符(生成GXT/字库的时候才干的)
    void CollectChars();

    void LoadBinary(const PathType& input_binary);
    void LoadText(const PathType& input_text);
    void LoadTexts(const PathType& input_texts);

    void GenerateTexts(const PathType& output_texts) const;
    void GenerateBinary(const PathType& output_binary) const;
    void GenerateCollection(const PathType& output_text) const;
    void GenerateTable(const PathType& output_binary) const;

    //文本/GXT形式互转
    static wStringType U8ToWide(const u8StringType& u8_string);
    static u8StringType WideToU8(const wStringType& wide_string);

    //修正原版GXT中的混乱字符
    static void FixCharacters(wStringType& wtext);

    //标准编码转到游戏编码
    static void LiteralToGame(wStringType& wtext);

    //游戏编码转到标准编码
    static void GameToLiteral(wStringType& wtext);

    //在中/英/Token之间添加空格(生成GXT的时候用)
    static wStringType SpaceCEKB(const wStringType& w_text);

    std::map<std::string, std::vector<TextEntry>, IVTextTableSorting> m_Data;
    std::set<char32_t> m_Chars;
};
