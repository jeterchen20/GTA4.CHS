#pragma once
#include "../common/stdinc.h"

struct IVTextTableSorting
{
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
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
    typedef std::uint16_t CharType;
    typedef std::uint32_t HashType;
    typedef tiny_utf8::utf8_string u8StringType;
    typedef std::basic_string<std::uint16_t> wStringType;
    typedef std::pair<HashType, u8StringType> EntryType;
    typedef std::pair<std::string, std::vector<EntryType>> TableType;
    typedef std::filesystem::path PathType;

    //不带参数，读取同一目录下GTA4.txt生成汉化补丁相关文件
    void Process0Arg();

    //文件, 目录，读取GXT文件并存放在指定目录下
    //目录, 目录，读取文件夹内所有txt生成汉化补丁相关文件
    void Process2Args(const PathType& arg1, const PathType& arg2);

private:
    //读取流时跳过UTF8签名
    static void SkipUTF8Signature(std::ifstream& stream);

    static bool IsNativeCharacter(char32_t character);

    //收集文本中需要加到字库的字符
    void CollectChars(const u8StringType& text);

    //收集文本中的Token
    void CollectTokens(const std::string& text, std::set<std::string>& tokens);

    void LoadBinary(const PathType& input_binary);
    void LoadText(const PathType& input_text);
    void LoadTexts(const PathType& input_texts);

    void GenerateTexts(const PathType& output_texts) const;
    void GenerateBinary(const PathType& output_binary) const;
    void GenerateCollection(const PathType& output_text) const;
    void GenerateTable(const PathType& output_binary) const;

    //修正原版GXT中的混乱字符
    static void FixCharacters(u8StringType& wtext);

    //标准编码转到游戏编码
    static void LiteralToGame(u8StringType& wtext);

    //游戏编码转到标准编码
    static void GameToLiteral(u8StringType& wtext);

    //在中/英/按键/BLIP之间添加空格(直接转换成写入gxt的状态)
    static wStringType SpaceCEKB(const u8StringType& u8_text);

    std::map<std::string, std::vector<EntryType>, IVTextTableSorting> m_Data;
    std::set<std::string> m_Tokens;
    std::set<char32_t> m_Chars;
};
