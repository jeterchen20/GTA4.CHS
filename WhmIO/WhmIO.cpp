// WhmIO.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "../common/stdinc.h"
#include "rscio.h"
#include "HTMLDocument.h"

void parse_document(MemoryFile& file)
{
    CHtmlDocument doc;

    doc.ReadDocument(file);

    auto strings = doc.GetStringList();

    for (auto& string : strings)
        std::cout << *string << std::endl;
}

void iterate_whm(const std::filesystem::path& folder)
{
    std::filesystem::recursive_directory_iterator it{ folder };

    for (; it != std::filesystem::recursive_directory_iterator{}; ++it)
    {
        if (!it->is_regular_file())
            continue;

        auto filename = it->path();

        if (filename.extension() == ".wtd")
            rscio::read_rsc(it->path());
    }
}

int main(int argc, char* argv[])
{
    iterate_whm("D:/Code/Git/GTA4.CHS/Debug/html");

#if 0
    input_path.replace_extension(".uncompressed");
    std::FILE* output_file;
    output_file = std::fopen(input_path.string().c_str(), "wb");
    if (output_file)
    {
        std::fwrite(uncompressed.data(), uncompressed.size(), 1, output_file);
        std::fclose(output_file);
    }
#endif

    //MemoryFile mem_file(std::move(uncompressed));
    //parse_document(mem_file);

    return 0;
}
