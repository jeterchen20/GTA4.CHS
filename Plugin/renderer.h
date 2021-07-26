#pragma once
#include "../common/stdinc.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

struct SCharTexture
{
    int gta_width; //��Ϸ�е��ַ����
    int width, height; //freetype��Ⱦ���Ĵ�С
    unsigned int texture_index; //�ǵڼ��������ϵ��ַ�
    int dx_width, dx_height; //IDirect3DTexture9����freetype��С����ȡ2^n
    unsigned char* pixels_pointer; //Image Watch�����ã�ָ��ԭʼ��������
    std::vector<unsigned char> pixels; //ԭʼ��������
    IDirect3DTexture9* dx_texture; //����
};

class CCharRenderer
{
public:
    static constexpr int FTPixelSize = 60;
    static constexpr int FTTopAdjust = 60;
    static constexpr int FTLeftAdjust = 2;
    static constexpr int GTABitmapWidth = 64;
    static constexpr int GTABitmapHeight = 80;

    CCharRenderer();
    CCharRenderer(CCharRenderer&& another) noexcept;
    CCharRenderer& operator=(CCharRenderer&& another) noexcept;
    ~CCharRenderer();

    CCharRenderer(const CCharRenderer&) = delete;
    CCharRenderer& operator=(const CCharRenderer&) = delete;

    bool FTInitialized() const;
    bool FaceLoaded() const;
    void CloseFT();

    void SetD3DDevice(IDirect3DDevice9* device);

    bool LoadFont(const std::filesystem::path& filename, int face_index = 0);
    void UnloadFont();

    const SCharTexture& LazyGetCharData(std::uint16_t code);

    void CacheChar(std::uint16_t code);
    void CacheChars(std::wstring_view wstr);

    std::size_t GetCacheCount() const;
    bool IsCacheEmpty() const;
    void ClearCache();

private:
    FT_Library m_ftLibrary;
    FT_Face m_ftFace;
    std::unordered_map<std::uint16_t, SCharTexture> m_charBuffer;
    SCharTexture m_specialChar;

    IDirect3DDevice9* m_d3dDevice;
    std::vector< IDirect3DTexture9*> m_largeTextures;

    bool m_isPRCMap; //����Ϊunicode

    bool InitializeFT();
    std::uint16_t Unicode2GB2312(std::uint16_t unic) const;
    SCharTexture MakeCharData(std::uint16_t code);
    SCharTexture MakeSpecialChar();
    const SCharTexture& GetSpecialChar();

    static float Fix26_6ToFloat(long fix_point);
    static float Fix16_16ToFloat(long fix_point);

    static IDirect3DTexture9* PixelsToTexture(IDirect3DDevice9* dev, const std::vector<unsigned char>& pixels, int width, int height);
};
