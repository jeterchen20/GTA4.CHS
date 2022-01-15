#include "../Plugin/save_title.h"

int main()
{
    uchar b_buffer[32];
    GTAChar w_buffer[512];

    misc_patch::gtaTruncateString(b_buffer, reinterpret_cast<const GTAChar*>(L"一二三四五六七八九十十一十二十三十四十五十六十七十八十九二十二十一二十二二十三二十四"), 32);
    misc_patch::gtaExpandString(b_buffer, w_buffer);
    misc_patch::gtaExpandString(reinterpret_cast<const uchar*>("test_string"), w_buffer);

    auto w_pointer_for_debug_view = reinterpret_cast<const wchar_t*>(w_buffer);

    return 0;
}
