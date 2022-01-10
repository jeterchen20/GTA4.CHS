#include "rscio.h"

std::vector<uchar> rscio::read_rsc(const std::filesystem::path& filename)
{
    std::vector<uchar> uncompressed_bytes;
    BinaryFile file(filename, "rb");

    if (file)
    {
        std::vector<uchar> compressed_bytes;
        RscHeader header;

        file.Seek(0, SEEK_END);
        auto compressed_size = file.Tell() - sizeof(header);
        compressed_bytes.resize(compressed_size);
        file.Seek(0, SEEK_SET);
        file.Read(header);

        auto cpu_size = header.flags.GetCpuSize();
        auto gpu_size = header.flags.GetGpuSize();
        ulong uncompressed_size = cpu_size + gpu_size;
        uncompressed_bytes.resize(uncompressed_size);
        file.Read(compressed_bytes.data(), compressed_size);

        uncompress(uncompressed_bytes.data(), &uncompressed_size, compressed_bytes.data(), compressed_size);
    }

    return uncompressed_bytes;
}

void rscio::write_rsc(const std::vector<uchar>& bytes, const std::filesystem::path& filename, uint virtual_size, uint physical_size)
{
    std::vector<uchar> compressed;
    auto compressed_size = compressBound(bytes.size());
    compressed.resize(compressed_size);
    compress(compressed.data(), &compressed_size, bytes.data(), bytes.size());

    RscHeader header;

    //构造header
    header.magic = 0x05435352; //"RSC"5
    header.type = rscio::RscType::Generic;
    header.flags.u.flags = 0;

    //TODO: 根据数据大小计算flags，单位页大小用原文件的值

    BinaryFile file;
    file.Open(filename, "wb");

    if (file)
    {
        file.Write(header);
        file.Write(compressed.data(), compressed_size);
    }
}
