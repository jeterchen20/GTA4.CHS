// WhmIO.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "../common/stdinc.h"
#include "rscio.h"
#include "HTMLDocument.h"

void parse_document(const buffer_type& buffer)
{
    CHtmlDocument doc;

    doc = *reinterpret_cast<const CHtmlDocument*>(buffer.data());

    std::cout << doc.m_pszTitle.read(buffer) << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        std::filesystem::path input_path = argv[1];
        auto uncompressed = rscio::read_rsc(input_path);
        input_path.replace_extension(".uncompressed");
        std::FILE* output_file;
        output_file = std::fopen(input_path.string().c_str(), "wb");
        if (output_file)
        {
            std::fwrite(uncompressed.data(), uncompressed.size(), 1, output_file);
            std::fclose(output_file);
        }

        parse_document(uncompressed);
    }

    return 0;
}
