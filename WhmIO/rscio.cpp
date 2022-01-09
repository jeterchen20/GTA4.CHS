#include "rscio.h"

std::vector<unsigned char> rscio::read_rsc(const std::filesystem::path& filename)
{
    std::vector<unsigned char> uncompressed_bytes;

    auto file_pointer = std::fopen(filename.string().c_str(), "rb");

    if (file_pointer)
    {
        std::vector<unsigned char> compressed_bytes;
        RscHeader header;

        std::fseek(file_pointer, 0, SEEK_END);
        auto compressed_size = std::ftell(file_pointer) - sizeof(header);
        compressed_bytes.resize(compressed_size);
        std::fseek(file_pointer, 0, SEEK_SET);
        std::fread(&header, sizeof(header), 1, file_pointer);

        auto virtualSize = (header.dwFlags & 0x7FF) << (((header.dwFlags >> 11) & 0xF) + 8);
        auto physicalSize = ((header.dwFlags >> 15) & 0x7FF) << (((header.dwFlags >> 26) & 0xF) + 8);
        auto uncompressed_size = virtualSize + physicalSize;
        uncompressed_bytes.resize(uncompressed_size);
        std::fread(compressed_bytes.data(), compressed_size, 1, file_pointer);

        uncompress(uncompressed_bytes.data(), &uncompressed_size, compressed_bytes.data(), compressed_size);
        std::fclose(file_pointer);
    }

    return uncompressed_bytes;
}

void rscio::write_rsc(const std::vector<unsigned char>& bytes, const std::filesystem::path& filename)
{

}
