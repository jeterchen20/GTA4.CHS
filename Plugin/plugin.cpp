#include "plugin.h"

#include "file_redirect.h"
#include "font.h"
#include "game.h"
#include "table.h"
#include "string_util.h"

CPlugin plugin;

static const char* __stdcall GetTextFileName(int)
{
    return "CHINESE.GXT";
}

void CPlugin::RegisterPatchSteps(batch_matching& batch_matcher)
{
    //获得变量和函数的地址
    batch_matcher.register_step("A1 ? ? ? ? 80 7C 24 08 00", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnGraphics_SetRenderState = addresses[0].p();
        });

    batch_matcher.register_step("0F B6 84 08 ? ? ? ? 66 0F 6E C0", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.pFont_Datas = *addresses[0].p<CFontInfo*>(4);
            game.game_addr.pFont_ButtonWidths = reinterpret_cast<float*>(&game.game_addr.pFont_Datas[4]);
        });

    batch_matcher.register_step("F3 0F 59 05 ? ? ? ? 88 4C 24 12", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.pFont_BlipWidth = *addresses[0].p<float*>(4);
        });

    batch_matcher.register_step("8B 34 ED ? ? ? ? 0F 2E C1", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.pFont_Details = *addresses[0].p<CFontDetails*>(3);
        });
    batch_matcher.register_step("81 3D ? ? ? ? AD 7F 33 31", 2, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.pFont_RenderState = *addresses[0].p<CFontRenderState*>(2);
        });
    batch_matcher.register_step("F3 0F 11 05 ? ? ? ? 66 0F 6E 84 24 AC 00 00", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.pFont_ResolutionX = *addresses[0].p<float*>(4);
        });
    batch_matcher.register_step("A1 ? ? ? ? 83 F8 FF 75 1E", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_GetRenderIndex = addresses[0].p();
        });
    batch_matcher.register_step("83 EC 30 83 3D ? ? ? ? FF", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_PrintChar = addresses[0].p();
        });
    batch_matcher.register_step("51 55 56 E8", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_GetCharacterSizeNormal = addresses[0].p();
        });
    batch_matcher.register_step("8A 0D ? ? ? ? 0F B6 D1", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_GetCharacterSizeDrawing = addresses[0].p();
        });
    batch_matcher.register_step("83 EC 10 8B 44 24 14 F3 0F 7E 00", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_Render2DPrimitive = addresses[0].p();
        });
    batch_matcher.register_step("8B 54 24 08 53 56 8B 74 24 0C 80 3E 22", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnHash_HashStringFromSeediCase = addresses[0].p();
        });
    batch_matcher.register_step("53 55 56 57 8B F9 85 FF 74 3F", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnDictionary_GetElementByKey = addresses[0].p();
        });

    batch_matcher.register_step("83 C7 02 53 57 E8", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_ParseToken = injector::GetBranchDestination(addresses[0].p(5)).get();
        });

    batch_matcher.register_step("83 EC 34 57 8B 7C 24 3C", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_ProcessToken = addresses[0].p();
        });

    batch_matcher.register_step("83 EC 10 53 8B 5C 24 18 55 56", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_AddTokenStringWidth = addresses[0].p();
        });

    batch_matcher.register_step("51 E8 ? ? ? ? 8D 04 C0 F3 0F 10 04 C5 ? ? ? ? F3 0F 59 05 ? ? ? ? F3 0F 59 04 C5", 1, [this](const byte_pattern::result_type& addresses)
        {
            game.game_addr.fnFont_GetActualLineHeight = addresses[0].p();
        });

    //获取字符串宽度
    batch_matcher.register_step("0F B7 06 83 F8 20", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(), CFont::GetStringWidthHook);
        });

    //跳过单词
    batch_matcher.register_step("57 8B 7C 24 08 85 FF 75 04 33 C0 5F C3 56", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeJMP(addresses[0].i(), CFont::SkipWord);
        });

    //获取字符宽度
    batch_matcher.register_step("83 C0 E0 50 E8 ? ? ? ? D9 5C 24", 2, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(4), CFont::GetCharacterSizeNormalDispatch);
            injector::MakeCALL(addresses[1].i(4), CFont::GetCharacterSizeNormalDispatch);
        });

    batch_matcher.register_step("6A 01 57 E8 ? ? ? ? D9 5C 24 30", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(3), CFont::GetCharacterSizeDrawingDispatch);
        });
    batch_matcher.register_step("EB 12 6A 01 57 E8 ? ? ? ? D9 5C 24 24", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(5), CFont::GetCharacterSizeDrawingDispatch);
        });

    //这些地方只读了一个字节..但是输入应该都是ascii
    //batch_matcher.registerStep("66 0F BE 06 0F B7 C0 50 E8", 1, [this](const byte_pattern::result_type &addresses) {injector::MakeCALL(addresses[0].i(8), CFont::GetCharacterSizeNormalDispatch); });
    //batch_matcher.registerStep("66 0F BE 04 1E 0F B7 C0 50 E8", 2, [this](const byte_pattern::result_type &addresses) {injector::MakeCALL(addresses[0].i(9), CFont::GetCharacterSizeNormalDispatch); injector::MakeCALL(addresses[1].i(9), CFont::GetCharacterSizeNormalDispatch); });

    //绘制字符
    batch_matcher.register_step("E8 ? ? ? ? 6A 01 57 E8 ? ? ? ? D9 5C 24 30", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(), CFont::PrintCharDispatch);
        });

    batch_matcher.register_step("F3 0F 11 0C 24 E8 ? ? ? ? 8B 35", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(5), CFont::PrintCharDispatch);
        });

    //加载fonts.wtd中的font_chs
    batch_matcher.register_step("8B CE 50 E8 ? ? ? ? 80 3D ? ? ? ? 6A", 2, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(3), CFont::LoadTextureCB);
            injector::MakeCALL(addresses[1].i(3), CFont::LoadTextureCB);
        });

    //改变GXT的文件名
    batch_matcher.register_step("80 7C 24 04 00 56 8B F1 74 07", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeJMP(addresses[0].i(), GetTextFileName);
        });

    //替换存档标题的字符串缩窄扩展函数
    batch_matcher.register_step("6A 3C 05 ? ? ? ? 50 68 ? ? ? ? E8", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(13), string_util::gtaTruncateString);
        });

    batch_matcher.register_step("8D 84 24 4C 01 00 00 50 8D 44 24 1C 50 E8", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(13), string_util::gtaExpandString);
        });

    //替换加载黑屏右下角的字符串扩展函数
    batch_matcher.register_step("FF 35 ? ? ? ? E8 ? ? ? ? 83 C4 0C 50 E8", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(15), string_util::gtaExpandString);
        });

    //替换邮件/网页的字符串缩窄扩展函数
    batch_matcher.register_step("8B F8 8D 44 24 08 50 57 E8", 1, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(8), string_util::gtaTruncateString2);
        });

    batch_matcher.register_step("74 0A 50 55 E8", 2, [this](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(4), string_util::gtaExpandString2);
            injector::MakeCALL(addresses[1].i(4), string_util::gtaExpandString2);
        });

    //邮件回复的逐字出现效果
    //batch_matcher.register_step("", 1, [this](const byte_pattern::result_type& addresses)
    //    {
    //
    //    });

    //Esc菜单Header间距

    //Esc菜单Header热区

    //手机右下角功能键异常换行(减小x坐标)

    //用plugins/GTA4.CHS/redirect/中存在的文件替代游戏文件
    batch_matcher.register_step("83 C4 04 50 FF 15 ? ? ? ? 8B F0", 6, [this](const byte_pattern::result_type& addresses)
        {
            injector::WriteMemory(*addresses[0].p<void*>(6), &RedirectCreateFileA, true);
        });

    //资料片文件重定向
}

std::filesystem::path CPlugin::GetModuleFolder(HMODULE m)
{
    wchar_t buffer[512];

    GetModuleFileNameW(m, buffer, 512);
    std::filesystem::path cpp_path{ buffer };
    return cpp_path.parent_path();
}

CPlugin::CPlugin()
{
    m_exe_module = GetModuleHandleW(NULL);
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(this), &m_plugin_module); //就先用着this吧

    m_exe_folder = GetModuleFolder(m_exe_module);
    m_plugin_folder = GetModuleFolder(m_plugin_module);
    m_redirect_folder = m_plugin_folder / "GTA4.CHS" / "redirect";

    m_init_succeeded = Init();
}

bool CPlugin::InitSucceeded() const
{
    return m_init_succeeded;
}

std::filesystem::path CPlugin::GetGameRoot() const
{
    return m_exe_folder;
}

std::filesystem::path CPlugin::GetRedirectRoot() const
{
    return m_redirect_folder;
}

std::filesystem::path CPlugin::GetPluginAsset(const char* rest_path) const
{
    return m_plugin_folder / "GTA4.CHS" / rest_path;
}

bool CPlugin::Init()
{
    batch_matching batch_matcher;

    RegisterPatchSteps(batch_matcher);

#if 1
    batch_matcher.perform_search();
#else
    batch_matcher.perform_search_mt();
#endif

    if (!batch_matcher.is_all_succeed())
        return false;

    redirector.scan_files();
    game.table.LoadTable(GetPluginAsset("table.dat"));

    batch_matcher.run_callbacks();

    return true;
}

HANDLE WINAPI RedirectCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
    return CreateFileA(plugin.redirector.redirect_path(lpFileName), dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
