#include "stdafx.h"
#include "Other.h"
#include "TMemory.h"
#include "Item.h"
#include "User.h"
#include "Controller.h"
#include "Interface.h"
#include "CheatGuard.h"
#include "Protocol.h"
#include "Monster.h"
#include "ResetSystem.h"
#include "ItemPrice.h"
#include "ChatExpanded.h"
#include "ConnectEx.h"
#include "TDebugLog.h"
#include "CraftSystem.h"
#include "VisualFix.h"
#include "OffTrade.h"
#include "ItemRank.h"
#include "ItemModel.h"
#include "SocketItem.h"
// ----------------------------------------------------------------------------------------------

extern "C" __declspec(dllexport)void Init()
{
	VMBEGIN
	// ----
	DWORD OldProtect;
	// ----
	if (VirtualProtect(LPVOID(0x401000), 0xD21FFF, PAGE_EXECUTE_READWRITE, &OldProtect))
	{
		gController.Load();
		gChatExpanded.Load();
		gItemPrice.Load();
		gItem.Load();
		gResetSystem.Load();
		gProtocol.Load();
		gCheatGuard.Load();
		gObjUser.Load();
		gOther.Load();
		gInterface.Load();
		gVisualFix.InitVisualFix();
		g_ItemModel.Load();
		gOffTrade.Init();
		// ----
		LoadLibrary("ttlci.dll");
		// ----
	//	gCraftSystem.Load();
	//	g_ItemRank.Load();
	//	gSocketItem.Load();
	}
	VMEND
}
// ----------------------------------------------------------------------------------------------

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_DETACH:
		{

		}
		break;
		// --
	case DLL_PROCESS_ATTACH:
		{
			gController.Instance = hModule;
		}
		break;
	}
	// ----
	return true;
}
// ----------------------------------------------------------------------------------------------