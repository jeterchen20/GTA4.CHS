#include "IVText.h"
#include "additional_chars.h"

void IVText::Process0Arg()
{
    wchar_t c_path[512];

    auto hmodule = GetModuleHandleW(NULL);

    ::GetModuleFileNameW(hmodule, c_path, 512);
    std::filesystem::path cpp_path{ c_path };
    auto root = cpp_path.parent_path();

    //text文件夹->gxt
    m_Data.clear();

    LoadTexts(root / "text");

    if (!m_Data.empty())
    {
        GenerateBinary(root / "chinese.gxt");
        CollectChars();
        GenerateCollection(root / "characters.txt");
        GenerateTable(root / "table.dat");
    }
}

void IVText::Process2Args(const PathType& arg1, const PathType& arg2)
{
    //指定路径的文件夹/gxt互转
    m_Data.clear();

    if (is_directory(arg1))
    {
        create_directories(arg2);
        LoadTexts(arg1);
        GenerateBinary(arg2 / "chinese.gxt");
        CollectChars();
        GenerateCollection(arg2 / "characters.txt");
        GenerateTable(arg2 / "table.dat");
    }
    else if (is_regular_file(arg1))
    {
        create_directories(arg2);
        LoadBinary(arg1);
        GenerateTexts(arg2);
    }
}

void IVText::SkipUTF8Signature(std::ifstream& stream)
{
    char bom[3];

    if (stream.get(bom[0]) && stream.get(bom[1]) && stream.get(bom[2]))
    {
        if (bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')
        {
            return;
        }
    }

    stream.seekg(0);
}

bool IVText::IsNativeCharacter(char32_t character)
{
    if (character > 0xFFFF)
        throw std::out_of_range("Invalid code point: larger than 0xFFFF.");

    return (character < 0x100 || character == L'™');
}

void IVText::CollectChars()
{
    m_Chars.clear();

    for (auto& table : m_Data)
    {
        for (auto& entry : table.second)
        {
            for (auto code_point : entry.text)
            {
                if (!IsNativeCharacter(code_point))
                {
                    m_Chars.insert(code_point);
                }
            }
        }
    }

    auto span_to_add = additional_chars::GetAddChars();

    m_Chars.insert(span_to_add.begin(), span_to_add.end());

    //排除不用记录的字符
    for (auto it = m_Chars.begin(); it != m_Chars.end();)
    {
        if (IsNativeCharacter(*it))
        {
            it = m_Chars.erase(it);
        }
        else
        {
            ++it;
        }
    }

    //排除全角空格
    m_Chars.erase(0x3000);
}

void IVText::LoadText(const PathType& input_text)
{
    std::regex table_regex(R"(\[([0-9a-zA-Z_]{1,7})\])");
    std::regex entry_regex(R"((0[xX][0-9a-fA-F]{8})=(.*))");
    std::regex origin_regex(R"(;(0[xX][0-9a-fA-F]{8})=(.*))");

    std::smatch matches;

    std::string line;

    auto table_iter = m_Data.end();
    std::string filename = input_text.filename().string();

    size_t line_no = 0;

    std::ifstream stream(input_text);

    if (!stream)
    {
        std::cout << "打开文件 " << input_text.string() << " 失败。" << std::endl;
        return;
    }

    SkipUTF8Signature(stream);

    while (std::getline(stream, line))
    {
        ++line_no;

        line.erase(0, line.find_first_not_of(' '));
        line.erase(line.find_last_not_of(' ') + 1);

        if (line.empty() || line.front() == ';')
        {

        }
        else if (line.front() == '0' && regex_match(line, matches, entry_regex))
        {
            if (table_iter != m_Data.end())
            {
                TextEntry entry;

                entry.hash = std::stoul(matches.str(1), nullptr, 16);
                auto b_string = matches.str(2);
                entry.text = b_string;

                if ((ranges::count(b_string, '~') % 2) != 0)
                {
                    std::cout << filename << ": " << "第" << line_no << "行的 \'~\' 个数不是偶数！" << std::endl;
                }

                table_iter->second.emplace_back(std::move(entry));
            }
            else
            {
                std::cout << filename << ": " << "第" << line_no << "行没有所属的表。" << std::endl;
            }
        }
        else if (line.front() == '[' && regex_match(line, matches, table_regex))
        {
            table_iter = m_Data.emplace(matches.str(1), std::vector<TextEntry>()).first;
        }
        else
        {
            std::cout << filename << ": " << "第" << line_no << "行无法识别。" << std::endl;
        }
    }
}

void IVText::LoadTexts(const PathType& input_texts)
{
    std::filesystem::directory_iterator dir_it{ input_texts };

    while (dir_it != std::filesystem::directory_iterator{})
    {
        PathType filename = dir_it->path();

        LoadText(filename);

        ++dir_it;
    }

    std::cout << "   " << std::endl;
}

void IVText::GenerateBinary(const PathType& output_binary) const
{
    BinaryFile file(output_binary, "wb");

    std::vector<uint8_t> buffer;
    long writePostion;

    GXTHeader gxtHeader;

    TableBlock tableBlock;
    KeyBlockOthers keyBlock;

    DataBlock dataBlock;

    TableEntry tableEntry;
    KeyEntry keyEntry;

    std::vector<TableEntry> tables;
    std::vector<KeyEntry> keys;
    std::vector<uint16_t> datas;

    if (!file)
    {
        std::cout << "创建输出文件 " << output_binary.string() << " 失败。" << std::endl;
        return;
    }

    gxtHeader.Version = 4;
    gxtHeader.CharBits = 16;
    file.Write(gxtHeader);

    tableBlock.TABL[0] = 'T';
    tableBlock.TABL[1] = 'A';
    tableBlock.TABL[2] = 'B';
    tableBlock.TABL[3] = 'L';

    keyBlock.Body.TKEY[0] = 'T';
    keyBlock.Body.TKEY[1] = 'K';
    keyBlock.Body.TKEY[2] = 'E';
    keyBlock.Body.TKEY[3] = 'Y';

    dataBlock.TDAT[0] = 'T';
    dataBlock.TDAT[1] = 'D';
    dataBlock.TDAT[2] = 'A';
    dataBlock.TDAT[3] = 'T';

    tableBlock.Size = m_Data.size() * sizeof(TableEntry);
    file.Write(tableBlock);

    writePostion = 4 + 8 + tableBlock.Size;

    tables.clear();

    for (auto& table : m_Data)
    {
        keys.clear();
        datas.clear();

        strcpy(tableEntry.Name, table.first.c_str());
        tableEntry.Offset = static_cast<int>(writePostion);
        tables.push_back(tableEntry);

        strcpy(keyBlock.Name, table.first.c_str());
        keyBlock.Body.Size = table.second.size() * sizeof(KeyEntry);

        for (auto& entry : table.second)
        {
            keyEntry.Hash = entry.hash;
            keyEntry.Offset = datas.size() * 2;

            auto w_string_to_write = SpaceCEKB(U8ToWide(entry.text));
            ranges::copy(w_string_to_write, std::back_inserter(datas));

            keys.push_back(keyEntry);
        }

        dataBlock.Size = datas.size() * 2;

        file.Seek(writePostion, SEEK_SET);

        if (table.first == "MAIN")
        {
            file.Write(keyBlock.Body);
        }
        else
        {
            file.Write(keyBlock);
        }

        file.WriteArray(keys);
        file.Write(dataBlock);
        file.WriteArray(datas);

        writePostion = file.Tell();
    }

    file.Seek(4 + 8, SEEK_SET);
    file.WriteArray(tables);
}

void IVText::GenerateCollection(const PathType& output_text) const
{
    size_t count = 0;
    u8StringType u8_text;

    for (auto character : m_Chars)
    {
        if (count == 64)
        {
            u8_text += L'\n';
            count = 0;
        }

        u8_text += character;

        ++count;
    }

    std::ofstream stream(output_text, std::ios::trunc);

    std::copy(u8_text.c_str(), u8_text.c_str() + u8_text.size(), std::ostreambuf_iterator<char>(stream));
}

void IVText::GenerateTable(const PathType& output_binary) const
{
    std::vector<CharacterData> datas;

    CharacterData data;

    data.pos.row = 0;
    data.pos.column = 0;

    for (auto chr : m_Chars)
    {
        data.code = chr;
        if (data.pos.column == 64)
        {
            ++data.pos.row;
            data.pos.column = 0;
        }

        datas.push_back(data);

        ++data.pos.column;
    }

    BinaryFile stream(output_binary, "wb");
    stream.WriteArray(datas);
}

IVText::wStringType IVText::U8ToWide(const u8StringType& u8_string)
{
    wStringType result;

    result.assign(u8_string.begin(), u8_string.end());
    result.push_back(0);
    LiteralToGame(result);

    return result;
}

IVText::u8StringType IVText::WideToU8(const wStringType& wide_string)
{
    u8StringType result;

    if (!wide_string.empty())
    {
        result.assign(wide_string.begin(), wide_string.end() - 1);
    }

    return result;
}

void IVText::FixCharacters(wStringType& wtext)
{
    //bad character in IV stock text: 0x85 0x92 0x94 0x96 0x97 0xA0
    //bad character in EFLC stock text: 0x93

    for (auto& character : wtext)
    {
        switch (character)
        {
        case 0x85:
            character = L' ';
            break;

        case 0x92:
        case 0x94:
            character = L'\'';
            break;

        case 0x93: //EFLC
            break;

        case 0x96:
            character = L'-';
            break;

        case 0x97:
        case 0xA0:
            character = L' ';
            break;

        default:
            break;
        }
    }
}

void IVText::LiteralToGame(wStringType& wtext)
{
    for (auto& character : wtext)
    {
        switch (character)
        {
        case L'™':
            character = 0x99;
            break;

        default:
            break;
        }
    }
}

void IVText::GameToLiteral(wStringType& wtext)
{
    for (auto& character : wtext)
    {
        switch (character)
        {
        case 0x99:
            character = L'™';
            break;

        default:
            break;
        }
    }
}

IVText::wStringType IVText::SpaceCEKB(const IVText::wStringType& w_text)
{
    //在重写排版函数之前的临时解决方案: 自动在分词的地方加空格

    //1. 保留原有空格
    //2. ~[A-Za-z]~/~COL_*~不是可绘制的token，保持原样
    //3. ~1~当作英语字符(如 '~1~%' / '%~1~'视作一个整体)
    //4. 保证汉字和非汉字之间是空格

    //以下Token保证两侧都是空格
    //~ACCEPT~
    //~BLIP_*~
    //~CANCEL~
    //~INPUT_*~
    //~MOUSE~
    //~MOUSE_WHEEL~
    //~PAD_*~
    enum class span_type
    {
        SPAN_PLACEHOLDER, //避免边界检查
        SPAN_SPACE, //一个空格
        SPAN_ZERO_WIDTH_TOKEN, //一个颜色类token
        SPAN_DRAWABLE_TOKEN, //一个按键/雷达之类的token
        SPAN_VALUE_PRINT_TOKEN, //数值格式化token，目前只有~1~
        SPAN_NATIVE_WORD, //一个英语单词('~1~'被当作一个英语字符)
        SPAN_CN_WORD //一个汉字
    };

    wStringType result;

    std::size_t text_index = 0;
    std::vector<std::pair<span_type, std::span<const GTAChar>>> string_spans;

    string_spans.emplace_back(span_type::SPAN_PLACEHOLDER, std::span<const GTAChar>{});
    //先对文本进行分词
    while (text_index < w_text.size() - 1)
    {
        auto c = w_text[text_index];

        if (c == 0)
            break;

        if (c == '~')
        {
            //一个Token
            auto token_end_index = text_index + 1;
            while (w_text[token_end_index] != '~')
            {
                ++token_end_index;
            }

            //~之间内容的长度
            auto token_string_length = token_end_index - text_index - 1;
            //基于已知的token内容简化逻辑
            if (token_string_length > 0)
            {
                if (token_string_length == 1)
                {
                    string_spans.emplace_back(
                        w_text[text_index + 1] == '1' ?
                        span_type::SPAN_VALUE_PRINT_TOKEN ://是~1~
                        span_type::SPAN_ZERO_WIDTH_TOKEN, //是~s~等单字符Token
                        std::span(&w_text[text_index], token_string_length + 2));
                }
                else
                {
                    if (w_text[text_index + 1] == 'C' && w_text[text_index + 2] == 'O')
                    {
                        //~COL_*~类型
                        string_spans.emplace_back(
                            span_type::SPAN_ZERO_WIDTH_TOKEN,
                            std::span(&w_text[text_index], token_string_length + 2));
                    }
                    else
                    {
                        //其他有宽度的类型
                        string_spans.emplace_back(
                            span_type::SPAN_DRAWABLE_TOKEN,
                            std::span(&w_text[text_index], token_string_length + 2));
                    }
                }
            }

            //跳过第二个'~'
            text_index = token_end_index + 1;
        }
        else if (c == ' ')
        {
            //一个空格
            string_spans.emplace_back(span_type::SPAN_SPACE, std::span(&w_text[text_index], 1));
            ++text_index;
        }
        else if (IsNativeCharacter(c))
        {
            //一个英语单词
            //搜索可能剩余的英语单词
            auto word_end_index = text_index + 1;
            while (IsNormalNativeChar(w_text[word_end_index]))
            {
                ++word_end_index;
            }

            string_spans.emplace_back(span_type::SPAN_NATIVE_WORD, std::span(&w_text[text_index], word_end_index - text_index));
            text_index = word_end_index;
        }
        else
        {
            //一个汉字
            string_spans.emplace_back(span_type::SPAN_CN_WORD, std::span(&w_text[text_index], 1));
            ++text_index;
        }
    }
    string_spans.emplace_back(span_type::SPAN_PLACEHOLDER, std::span<const GTAChar>{});

    std::size_t span_index = 1;
    for (; span_index < string_spans.size() - 1; ++span_index)
    {
        auto append_span_to_result = [&string_spans, &span_index, &result]
        {
            for (auto& span_element : string_spans[span_index].second)
            {
                result.push_back(span_element);
            }
        };

        append_span_to_result();

        //根据前后span的类型决定是否要跟随空格
        auto prev_span_type = string_spans[span_index - 1].first;
        auto next_span_type = string_spans[span_index + 1].first;
        switch (string_spans[span_index].first)
        {
        case span_type::SPAN_PLACEHOLDER:
        {
            break;
        }

        case span_type::SPAN_SPACE:
        {
            break;
        }

        //为了处理 "汉字~s~~BLIP_24~" 这类情况才写得这么哆嗦
        case span_type::SPAN_ZERO_WIDTH_TOKEN:
        {
            if ((((prev_span_type != span_type::SPAN_PLACEHOLDER && prev_span_type != span_type::SPAN_ZERO_WIDTH_TOKEN && prev_span_type != span_type::SPAN_CN_WORD) &&
                (next_span_type != span_type::SPAN_PLACEHOLDER && next_span_type != span_type::SPAN_ZERO_WIDTH_TOKEN && next_span_type != span_type::SPAN_CN_WORD)) ||
                ((prev_span_type != span_type::SPAN_PLACEHOLDER && prev_span_type != span_type::SPAN_ZERO_WIDTH_TOKEN) &&
                    (next_span_type != span_type::SPAN_PLACEHOLDER && next_span_type != span_type::SPAN_ZERO_WIDTH_TOKEN))) && next_span_type != span_type::SPAN_SPACE)
                result.push_back(' ');

            break;
        }

        case span_type::SPAN_VALUE_PRINT_TOKEN:
        {
            if ((next_span_type == span_type::SPAN_CN_WORD ||
                next_span_type == span_type::SPAN_DRAWABLE_TOKEN) && next_span_type != span_type::SPAN_SPACE)
                result.push_back(' ');

            break;
        }

        case span_type::SPAN_DRAWABLE_TOKEN:
        {
            if (next_span_type != span_type::SPAN_PLACEHOLDER && next_span_type != span_type::SPAN_SPACE)
                result.push_back(' ');

            break;
        }

        case span_type::SPAN_NATIVE_WORD:
        {
            if (next_span_type != span_type::SPAN_PLACEHOLDER &&
                next_span_type != span_type::SPAN_SPACE &&
                next_span_type != span_type::SPAN_VALUE_PRINT_TOKEN)
                result.push_back(' ');

            break;
        }

        case span_type::SPAN_CN_WORD:
        {
            if ((next_span_type == span_type::SPAN_VALUE_PRINT_TOKEN ||
                next_span_type == span_type::SPAN_NATIVE_WORD ||
                next_span_type == span_type::SPAN_DRAWABLE_TOKEN) && next_span_type != span_type::SPAN_SPACE)
                result.push_back(' ');

            break;
        }

        default:
        {
            throw std::invalid_argument("Unexpected span_type.");
        }
        }
    }

    //TODO: 将连续的空格变成一个

    result.push_back(0);
    auto w_pointer_for_debug_view = reinterpret_cast<wchar_t*>(result.data());

    return result;
}

void IVText::LoadBinary(const PathType& input_binary)
{
    GXTHeader gxtHeader;
    TableBlock tableBlock;
    KeyBlockOthers keyBlock;
    DataBlock tdatHeader;

    std::vector<TableEntry> tables;
    std::vector<KeyEntry> keys;
    std::vector<CharType> datas;

    m_Data.clear();

    auto tableIter = m_Data.end();

    BinaryFile file(input_binary, "rb");

    if (!file)
    {
        std::cout << "打开输入文件 " << input_binary.string() << " 失败。" << std::endl;
        return;
    }

    file.Read(gxtHeader);

    file.Read(tableBlock);

    file.ReadArray(tableBlock.Size / sizeof(TableEntry), tables);

    for (TableEntry& table : tables)
    {
        tableIter = m_Data.emplace(table.Name, std::vector<TextEntry>()).first;

        file.Seek(table.Offset, SEEK_SET);

        if (strcmp(table.Name, "MAIN") != 0)
        {
            file.Read(keyBlock);
        }
        else
        {
            file.Read(keyBlock.Body);
        }

        file.ReadArray(keyBlock.Body.Size / sizeof(KeyEntry), keys);

        file.Read(tdatHeader);

        file.ReadArray(tdatHeader.Size / 2, datas);

        for (auto& key : keys)
        {
            wStringType w_string;
            TextEntry entry;

            entry.hash = key.Hash;
            auto offset = key.Offset / 2;

            while (datas[offset] != 0)
            {
                w_string.push_back(datas[offset]);
                ++offset;
            }
            w_string.push_back(0);

            FixCharacters(w_string);
            GameToLiteral(w_string);
            entry.text = WideToU8(w_string);

            tableIter->second.emplace_back(std::move(entry));
        }
    }
}

void IVText::GenerateTexts(const PathType& output_texts) const
{
    std::ofstream stream;
    std::string line;

    for (auto& table : m_Data)
    {
        stream.open(output_texts / (table.first + ".txt"), std::ios::trunc);

        if (!stream)
        {
            std::cout << "创建输出文件失败" << std::endl;
        }

        stream.write("\xEF\xBB\xBF", 3);

        stream << fmt::sprintf("[%s]\n", table.first);

        for (auto& entry : table.second)
        {
            line = fmt::sprintf("0x%08X=%s\n", entry.hash, entry.text.c_str());
            stream << ';' << line << line << '\n';
        }

        stream.close();
    }
}
