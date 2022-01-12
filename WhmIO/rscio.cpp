#include "rscio.h"

rsc_blocks rscio::read_rsc(const std::filesystem::path& filename)
{
    rsc_blocks blocks;
    BinaryFile file(filename, "rb");

    if (file)
    {
        std::vector<uchar> compressed_bytes, uncompressed_bytes;
        rsc_header header;

        file.Seek(0, SEEK_END);
        auto compressed_size = file.Tell() - sizeof(header);
        compressed_bytes.resize(compressed_size);
        file.Seek(0, SEEK_SET);
        file.Read(header);

        std::cout << fmt::sprintf("virtual page size:%u, virtual size: %u", header.flags.GetVirtualPageSize(), header.flags.GetVirtualPageSize()) << std::endl;
        std::cout << fmt::sprintf("physical page size:%u, physical size: %u", header.flags.GetPhysicalPageSize(), header.flags.GetPhysicalPageSize()) << std::endl;
        std::cout << std::endl;
        return blocks;

        auto virtual_size = header.flags.GetVirtualSize();
        auto physical_size = header.flags.GetPhysicalSize();
        ulong uncompressed_size = virtual_size + physical_size;
        blocks.virtual_block.resize(virtual_size);
        blocks.physical_block.resize(physical_size);
        uncompressed_bytes.resize(uncompressed_size);
        file.Read(compressed_bytes.data(), compressed_size);

        uncompress(uncompressed_bytes.data(), &uncompressed_size, compressed_bytes.data(), compressed_size);
        //将解压后的数据按照大小分块
        std::memcpy(blocks.virtual_block.data(), compressed_bytes.data(), virtual_size);
        std::memcpy(blocks.physical_block.data(), compressed_bytes.data() + virtual_size, physical_size);
    }

    return blocks;
}

void rscio::write_rsc(const rsc_blocks& blocks, const std::filesystem::path& filename)
{

}
