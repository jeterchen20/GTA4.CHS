#include "../Plugin/string_util.h"

void Test()
{
    uchar bbuffer[256];
    wchar_t wbuffer[256];

    string_util::gtaTruncateString(bbuffer, reinterpret_cast<const GTAChar*>(L"Bellic 兄弟"), 256);
    string_util::gtaExpandString(bbuffer, reinterpret_cast<GTAChar*>(wbuffer));

    auto p = wbuffer;

    return;
}

int main()
{
    Test();

    return 0;
}
