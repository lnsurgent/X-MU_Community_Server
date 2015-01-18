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
#ifdef __NOVUS__
#include "CraftSystem.h"
#endif
#include "VisualFix.h"
#include "OffTrade.h"
#include "ItemRank.h"
#include "ItemModel.h"
#include "SocketItem.h"
#include <string>
#include "CRC.h"
// ----------------------------------------------------------------------------------------------
std::vector<std::string> GetArguments()
{
	std::vector<std::string> list;
	LPSTR arguments = GetCommandLineA();
	if(!arguments)
	{
		return list;
	}
	size_t i = 0;
	bool quote = false;
	bool it = false;
	std::string argument = "";
	std::string commandline = arguments;
	while(i < commandline.length())
	{
		if(quote)
		{
			if(commandline.at(i) == '\"')
			{
				quote = false;
			}
			else
			{
				argument += commandline.at(i);
				it = true;
			}
		}
		else
		{
			switch(commandline.at(i))
			{
			case '\"':
				quote = true;
				break;
			case ' ': 
			case '\t':
			case '\r':
			case '\n':
				if(it)
				{
					list.push_back(argument);
					argument = "";
					it = false;
				}
				break;
			default:
				argument += commandline.at(i);
				it = true;
				break;
			}
		}
		i++;
	}
	if(argument != "")
	{
		list.push_back(argument);
	}
	return list;
}

void CheckArguments()
{
	std::vector<std::string> argumentos = GetArguments();
	if(argumentos.size() == 2)
	{
		if(argumentos[1] == "season6globalextreme")
		{
			return;
		}
	}
	ShellExecute(GetDesktopWindow(), "open", "Launcher.exe", NULL, NULL, SW_SHOWNORMAL);
	ExitProcess(0);
}

extern "C" __declspec(dllexport)void Init()
{
	// ----
#ifdef __MUANGEL__
	if( pMUIsLoaded == 1 )
	{
		CreateMutex(0, 1, "MuOnline");
		// ----
		if( GetLastError() == 183 )
		{
			ExitProcess(0);
		}
	}
#endif
	// ----
#if defined  __MIX__ || __WHITE__ || __MUANGEL__
#ifndef __ROOT__
	char **	Command	= 0;
	CommandLineToArg(GetCommandLine(), &Command);
	if( strcmp("Updater", Command[1]) )
	{
		MessageBoxA(0, "Please start game from Launcher", "Start Error", ERROR);
		ExitProcess(0);
	}
#endif
#endif
	// ----
	// ----
#ifdef __NOVUS__
	gCraftSystem.Load();
#endif
	//#if defined __BEREZNUK__ || __MIX__ || __REEDLAN__ || __MUANGEL__ || __WHITE__ || __MEGAMU__ || __VIRNET__
	gConnectEx.Load();
	//#endif
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
	//#ifdef __ALIEN__
	gOffTrade.Init();
	//#endif
#ifdef __RMOS__
	g_ItemRank.Load(); 
#endif
	// ----
	LoadLibrary("ttlci.dll");	
#ifdef MUNEWAGE
	CheckArguments();
	
#endif
	
	// ----
	//gSocketItem.Load();
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