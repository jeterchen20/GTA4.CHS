﻿#include "plugin.h"
#include "font.h"
#include "table.h"
#include "game.h"
#include "batch_matching.h"

static const char* __stdcall GetTextFileName(int)
{
    return "CHINESE.GXT";
}

static void RegisterPatchSteps(batch_matching &batch_matcher)
{
    //变量和函数的地址
    batch_matcher.register_step("A1 ? ? ? ? 80 7C 24 08 00", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pGraphics_SetRenderState = addresses[0].p();
        });

    batch_matcher.register_step("0F B6 84 08 ? ? ? ? 66 0F 6E C0", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_Datas = *addresses[0].p<CFontInfo*>(4);
            CGame::Addresses.pFont_ButtonWidths = reinterpret_cast<float*>(&CGame::Addresses.pFont_Datas[4]);
        });

    batch_matcher.register_step("F3 0F 59 05 ? ? ? ? 88 4C 24 12", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_BlipWidth = *addresses[0].p<float*>(4);
        });

    batch_matcher.register_step("8B 34 ED ? ? ? ? 0F 2E C1", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_Details = *addresses[0].p<CFontDetails*>(3);
        });
    batch_matcher.register_step("81 3D ? ? ? ? AD 7F 33 31", 2, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_RenderState = *addresses[0].p<CFontRenderState*>(2);
        });
    batch_matcher.register_step("F3 0F 11 05 ? ? ? ? 66 0F 6E 84 24 AC 00 00", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_ResolutionX = *addresses[0].p<float*>(4);
        });
    batch_matcher.register_step("A1 ? ? ? ? 83 F8 FF 75 1E", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_GetRenderIndex = addresses[0].p();
        });
    batch_matcher.register_step("83 EC 30 83 3D ? ? ? ? FF", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_PrintChar = addresses[0].p();
        });
    batch_matcher.register_step("51 55 56 E8", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_GetCharacterSizeNormal = addresses[0].p();
        });
    batch_matcher.register_step("8A 0D ? ? ? ? 0F B6 D1", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_GetCharacterSizeDrawing = addresses[0].p();
        });
    batch_matcher.register_step("83 EC 10 8B 44 24 14 F3 0F 7E 00", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_Render2DPrimitive = addresses[0].p();
        });
    batch_matcher.register_step("8B 54 24 08 53 56 8B 74 24 0C 80 3E 22", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pHash_HashStringFromSeediCase = addresses[0].p();
        });
    batch_matcher.register_step("53 55 56 57 8B F9 85 FF 74 3F", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pDictionary_GetElementByKey = addresses[0].p();
        });

    batch_matcher.register_step("83 C7 02 53 57 E8", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_ParseToken = injector::GetBranchDestination(addresses[0].p(5)).get();
        });

    batch_matcher.register_step("83 EC 34 57 8B 7C 24 3C", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_ProcessToken = addresses[0].p();
        });

    batch_matcher.register_step("83 EC 10 53 8B 5C 24 18 55 56", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_AddTokenStringWidth = addresses[0].p();
        });

    batch_matcher.register_step("51 E8 ? ? ? ? 8D 04 C0 F3 0F 10 04 C5 ? ? ? ? F3 0F 59 05 ? ? ? ? F3 0F 59 04 C5", 1, [](const byte_pattern::result_type& addresses)
        {
            CGame::Addresses.pFont_GetActualLineHeight = addresses[0].p();
        });

    //GetMaxWordWidth
    batch_matcher.register_step("51 56 8B 74 24 0C 85 F6 75 05 D9 EE 5E 59 C3 66", 1, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeJMP(addresses[0].i(), CFont::GetMaxWordWidth);
        });

    //GetStringWidth
    batch_matcher.register_step("B8 B4 10 00 00", 1, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeJMP(addresses[0].i(-6), CFont::GetStringWidth);
        });

    //ProcessString
    batch_matcher.register_step("81 EC 8C 0A 00 00", 1, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeJMP(addresses[0].i(), CFont::ProcessString);
        });

    //获取字符宽度
    batch_matcher.register_step("83 C0 E0 50 E8 ? ? ? ? D9 5C 24", 2, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(4), CFont::GetCharacterSizeNormalDispatch);
            injector::MakeCALL(addresses[1].i(4), CFont::GetCharacterSizeNormalDispatch);
        });

    batch_matcher.register_step("6A 01 57 E8 ? ? ? ? D9 5C 24 30", 1, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(3), CFont::GetCharacterSizeDrawingDispatch);
        });
    batch_matcher.register_step("EB 12 6A 01 57 E8 ? ? ? ? D9 5C 24 24", 1, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(5), CFont::GetCharacterSizeDrawingDispatch);
        });

    //这些地方只读了一个字节..但是输入应该都是ascii
    //batch_matcher.registerStep("66 0F BE 06 0F B7 C0 50 E8", 1, [](const byte_pattern::result_type &addresses) {injector::MakeCALL(addresses[0].i(8), CFont::GetCharacterSizeNormalDispatch); });
    //batch_matcher.registerStep("66 0F BE 04 1E 0F B7 C0 50 E8", 2, [](const byte_pattern::result_type &addresses) {injector::MakeCALL(addresses[0].i(9), CFont::GetCharacterSizeNormalDispatch); injector::MakeCALL(addresses[1].i(9), CFont::GetCharacterSizeNormalDispatch); });

    //绘制字符
    batch_matcher.register_step("E8 ? ? ? ? 6A 01 57 E8 ? ? ? ? D9 5C 24 30", 1, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(), CFont::PrintCharDispatch);
        });

    batch_matcher.register_step("F3 0F 11 0C 24 E8 ? ? ? ? 8B 35", 1, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(5), CFont::PrintCharDispatch);
        });


    //加载font_chs，即中文字库
    batch_matcher.register_step("8B CE 50 E8 ? ? ? ? 80 3D ? ? ? ? 6A", 2, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeCALL(addresses[0].i(3), CFont::LoadTextureCB);
            injector::MakeCALL(addresses[1].i(3), CFont::LoadTextureCB);
        });

    //改变GXT的文件名
    batch_matcher.register_step("80 7C 24 04 00 56 8B F1 74 07", 1, [](const byte_pattern::result_type& addresses)
        {
            injector::MakeJMP(addresses[0].i(), GetTextFileName);
        });

    //Esc菜单抬头热区

    //存档名字读双字节

}

bool CPlugin::Init(HMODULE module)
{
    wchar_t PluginPath[512];
    GetModuleFileNameW(module, PluginPath, 512);

    batch_matching batch_matcher;

    RegisterPatchSteps(batch_matcher);

    batch_matcher.perform_search();
    if (batch_matcher.is_all_succeed())
    {
        GlobalTable.LoadTable(std::filesystem::path{ PluginPath }.parent_path() / "GTA4.CHS/table.dat");
        batch_matcher.run_callbacks();
        return true;
    }
    else
    {
        return false;
    }
}
