#pragma once
#include <cstddef>
#include <filesystem>
#include <cstdio>
#include <type_traits>
#include <vector>

class BinaryFile
{
public:
    BinaryFile() :m_pFile(nullptr) {}
    BinaryFile(const BinaryFile&) = delete;
    BinaryFile& operator=(const BinaryFile&) = delete;

    BinaryFile(BinaryFile&& rv) noexcept
        :BinaryFile()
    {
        std::exchange(m_pFile, rv.m_pFile);
    }

    BinaryFile& operator=(BinaryFile&& rv) noexcept
    {
        Close();

        std::exchange(m_pFile, rv.m_pFile);
    }

    BinaryFile(const std::filesystem::path& filename, const char* mode)
        :BinaryFile()
    {
        Open(filename, mode);
    }

    bool Open(const std::filesystem::path& filename, const char* mode)
    {
        Close();

        m_pFile = std::fopen(filename.string().c_str(), mode);
        return (m_pFile != nullptr);
    }

    void Close()
    {
        if (m_pFile != nullptr)
        {
            std::fclose(m_pFile);
            m_pFile = nullptr;
        }
    }

    bool Opened() const
    {
        return m_pFile != nullptr;
    }

    operator bool() const
    {
        return Opened();
    }

    BinaryFile& Seek(long offset, int mode)
    {
        fseek(m_pFile, offset, mode);

        return *this;
    }

    long Tell() const
    {
        return ftell(m_pFile);
    }

    BinaryFile& Read(void* buffer, std::size_t size)
    {
        std::fread(buffer, size, 1, m_pFile);
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_trivially_copyable_v<T>, BinaryFile&> Read(T& object)
    {
        Read(&object, sizeof(object));
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_trivially_copyable_v<T>, BinaryFile&> ReadArray(std::size_t count, std::vector<T>& objects)
    {
        objects.resize(count);
        Read(objects.data(), sizeof(T) * count);
        return *this;
    }

    BinaryFile& Write(const void* buffer, std::size_t size)
    {
        std::fwrite(buffer, size, 1, m_pFile);
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_trivially_copyable_v<T>, BinaryFile&> Write(const T& object)
    {
        Write(&object, sizeof(object));
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_trivially_copyable_v<T>, BinaryFile&> WriteArray(const std::vector<T>& objects)
    {
        Write(objects.data(), sizeof(T) * objects.size());
        return *this;
    }

private:
    std::FILE* m_pFile;
};
