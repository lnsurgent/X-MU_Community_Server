#include "StdAfx.h"
#include "OfflineTrade.h"
#include "ConnectEx.h"
#include "giocp.h"
#include "SProtocol.h"
#include "user.h"
#include "GameMain.h"
#include "..\include\ReadScript.h"
#include "..\common\winutil.h"
#include "logproc.h"
#include "LogToFile.h"
//#ifdef __ALIEN__
#include "DSProtocol.h"
//#endif
// -------------------------------------------------------------------------------

CLogToFile g_OfflineTradeLog("Reset", ".\\LOG\\OfflineTrade", TRUE);
// -------------------------------------------------------------------------

OfflineTrade g_OfflineTrade;
// -------------------------------------------------------------------------------

OfflineTrade::OfflineTrade()
{
	this->Init();
}
// -------------------------------------------------------------------------------

OfflineTrade::~OfflineTrade()
{

}
// -------------------------------------------------------------------------------

void OfflineTrade::Init()
{
	this->m_ReqLevel		= 1;
	this->m_ReqReset		= 0;
	this->m_UseMapList		= false;
	this->m_LoadedMapList	= 0;
	// ----
	for( int i = 0; i < MAX_NUMBER_MAP; i++ )
	{
		this->m_MapList[i] = -1;
	}
	// ----
	ZeroMemory(this->m_Cost, sizeof(this->m_Cost));
}
// -------------------------------------------------------------------------------

void OfflineTrade::Load()
{
	this->Init();
	this->ReadData(gDirPath.GetNewPath("Custom\\OfflineTrade.txt"));
}
// -------------------------------------------------------------------------------

void OfflineTrade::ReadData(char * File)
{
	SMDToken Token;
	SMDFile = fopen(File, "r");
	// ----
	if( !SMDFile )
	{
		MsgBox("[OfflineTrade] %s file not found", File);
		return;
	}
	// ----
	BYTE WCoinCCounter = 0, WCoinPCounter = 0, GPCounter = 0;
	int Category = -1;
	// ----
	while(true)
	{
		Token = GetToken();
		// ----
		if( Token == END )
		{
			break;
		}
		// ----
		Category = TokenNumber;
		// ----
		while(true)
		{
			if( Category == 0 )	//-> Requirements
			{
				Token = GetToken();
				// ----
				if( !strcmp("end", TokenString) )
				{
					break;
				}
				// ----
				this->m_ReqLevel	= TokenNumber;
				// ----
				Token = GetToken();
				this->m_ReqReset	= TokenNumber;
				// ----
				Token = GetToken();
				this->m_UseMapList	= TokenNumber;
			}
			else if( Category == 1 ) //-> Cost
			{
				Token = GetToken();
				// ----
				if( !strcmp("end", TokenString) )
				{
					break;
				}
				// ----
				this->m_Cost[0]		= TokenNumber;
				// ----
				Token = GetToken();
				this->m_Cost[1]		= TokenNumber;
				// ----
				Token = GetToken();
				this->m_Cost[2]		= TokenNumber;
				// ----
				Token = GetToken();
				this->m_Cost[3]		= TokenNumber;
			}
			else if( Category == 2 ) //-> Map list
			{
				Token = GetToken();
				// ----
				if( !strcmp("end", TokenString) )
				{
					break;
				}
				// ----
				this->m_MapList[this->m_LoadedMapList] = TokenNumber;
				this->m_LoadedMapList++;
			}
			//#ifdef __ALIEN__
			else if( Category == 3 )	//-> Enable OffTrade
			{
				Token = GetToken();
				// ----
				if( !strcmp("end", TokenString) )
				{
					break;
				}
				// ----
				this->EnableZen	= TokenNumber;
				// ----
				Token = GetToken();
				this->EnableWCoin	= TokenNumber;
				// ----
				Token = GetToken();
				this->EnableSoul	= TokenNumber;
				// ----
				Token = GetToken();
				this->EnableBless	= TokenNumber;
				// ----
				Token = GetToken();
				this->EnableChaos	= TokenNumber;
				// ----
				Token = GetToken();
				this->EnableLife	= TokenNumber;
				// ----
				Token = GetToken();
				this->EnableCreation	= TokenNumber;
			}
			else if( Category == 4 )	//-> tax for wCoin OffTrade
			{
				Token = GetToken();
				// ----
				if( !strcmp("end", TokenString) )
				{
					break;
				}
				// ----
				this->m_wCoinTax	= TokenNumber;
				// ----
			}
			//#endif
		}
	}
	// ----
	fclose(SMDFile);
	LogAddTD("[OfflineTrade] %s file is loaded (MapList: %d (%d))", File, this->m_UseMapList, this->m_LoadedMapList);
}
// -------------------------------------------------------------------------------

bool OfflineTrade::CheckReq(int UserIndex)
{
	LPOBJ lpUser = &gObj[UserIndex];
	// ----
	if( !lpUser->m_bPShopOpen )
	{
		GCServerMsgStringSend("You need open p. shop to create offline store", UserIndex, 1);
		g_OfflineTradeLog.Output("[%s] [%s] Request aborted - not opened shop", 
			lpUser->AccountID, lpUser->Name);
		return false;
	}
	// ----
	if( lpUser->Level < this->m_ReqLevel )
	{
		GCServerMsgStringSend("Your Level is small to create offline store", UserIndex, 1);
		g_OfflineTradeLog.Output("[%s] [%s] Request aborted - small level (%d / %d)", 
			lpUser->AccountID, lpUser->Name, lpUser->Level, this->m_ReqLevel);
		return false;
	}
	// ----
	if( lpUser->Reset < this->m_ReqReset )
	{
		GCServerMsgStringSend("Your Reset is small to create offline store", UserIndex, 1);
		g_OfflineTradeLog.Output("[%s] [%s] Request aborted - small reset (%d / %d)", 
			lpUser->AccountID, lpUser->Name, lpUser->Reset, this->m_ReqReset);
		return false;
	}
	// ----
	if( lpUser->Money < this->m_Cost[0] )
	{
		GCServerMsgStringSend("You need more Zen to create offline store", UserIndex, 1);
		g_OfflineTradeLog.Output("[%s] [%s] Request aborted - short money (%d / %d)", 
			lpUser->AccountID, lpUser->Name, lpUser->Money, this->m_Cost[0]);
		return false;
	}
	// ----
	if( lpUser->GameShop.WCoinC < this->m_Cost[1] )
	{
		GCServerMsgStringSend("You need more WCoinC to create offline store", UserIndex, 1);
		g_OfflineTradeLog.Output("[%s] [%s] Request aborted - short wcoinc (%d / %d)", 
			lpUser->AccountID, lpUser->Name, lpUser->GameShop.WCoinC, this->m_Cost[1]);
		return false;
	}
	// ----
	if( lpUser->GameShop.WCoinP < this->m_Cost[2] )
	{
		GCServerMsgStringSend("You need more WCoinP to create offline store", UserIndex, 1);
		g_OfflineTradeLog.Output("[%s] [%s] Request aborted - short wcoinp (%d / %d)", 
			lpUser->AccountID, lpUser->Name, lpUser->GameShop.WCoinP, this->m_Cost[2]);
		return false;
	}
	// ----
	if( lpUser->GameShop.GoblinPoint < this->m_Cost[3] )
	{
		GCServerMsgStringSend("You need more GoblinPoint to create offline store", UserIndex, 1);
		g_OfflineTradeLog.Output("[%s] [%s] Request aborted - short wcoing (%d / %d)", 
			lpUser->AccountID, lpUser->Name, lpUser->GameShop.GoblinPoint, this->m_Cost[3]);
		return false;
	}
	// ----
	if( this->m_UseMapList )
	{
		bool RightMap = false;
		// ----
		for( int i = 0; i < MAX_NUMBER_MAP; i++ )
		{
			if( this->m_MapList[i] == lpUser->MapNumber )
			{
				RightMap = true;
				break;
			}
		}
		// ----
		if( !RightMap )
		{
			GCServerMsgStringSend("You can't open offline store on this map", UserIndex, 1);
			g_OfflineTradeLog.Output("[%s] [%s] Request aborted - invalid map (%d)", 
				lpUser->AccountID, lpUser->Name, lpUser->MapNumber);
			return false;
		}
	}
	// ----
	return true;
}
// -------------------------------------------------------------------------------

//#ifdef __ALIEN__
void OfflineTrade::Start(int UserIndex, int Type)
	//#else
	//void OfflineTrade::Start(int UserIndex)
	//#endif
{
	LPOBJ lpUser = &gObj[UserIndex];
	// ----
	g_OfflineTradeLog.Output("[%s] [%s] Send request to start offline trade", 
		lpUser->AccountID, lpUser->Name);
	// ----
	if( !this->CheckReq(UserIndex) )
	{
		return;
	}
	//#ifdef __ALIEN__
	switch(Type)
	{
	case 0: strcpy(lpUser->m_szPShopText, ZEN_SHOP);		break;
	case 1: strcpy(lpUser->m_szPShopText, WCOIN_SHOP);	break;
	case 2: strcpy(lpUser->m_szPShopText, SOUL_SHOP);	break;
	case 3: strcpy(lpUser->m_szPShopText, BLESS_SHOP);	break;
	case 4:	strcpy(lpUser->m_szPShopText, CHAOS_SHOP);	break;
	case 5:	strcpy(lpUser->m_szPShopText, LIFE_SHOP);	break;
	case 6:	strcpy(lpUser->m_szPShopText, CREATION_SHOP);	break;
	default:return;
	}
	//#endif
	// ----
	lpUser->Money					-= this->m_Cost[0];
	lpUser->GameShop.WCoinC			-= this->m_Cost[1];
	lpUser->GameShop.WCoinP			-= this->m_Cost[2];
	lpUser->GameShop.GoblinPoint	-= this->m_Cost[3];
	// ----
	//#if defined __BEREZNUK__ || __MIX__ || __REEDLAN__ || __MUANGEL__ || __WHITE__ || __MEGAMU__ || __VIRNET__
	if(g_AutoReconnect == 1)
	{
		g_ConnectEx.SendClose(UserIndex);
	}
	//#endif
	// ----
	gGameShop.GDSavePoint(UserIndex);
	GCMoneySend(UserIndex, lpUser->Money);
	lpUser->IsOffTrade = true;
	CloseClientEx(UserIndex);
	// ----
	//#ifdef __ALIEN__
	g_OfflineTradeLog.Output("[%s] [%s] Has been disconnected, offline shop created (Type: %d)", 
		lpUser->AccountID, lpUser->Name, Type);
	//#else
	//	g_OfflineTradeLog.Output("[%s] [%s] Has been disconnected, offline shop created", 
	//		lpUser->AccountID, lpUser->Name);
	//#endif
}
// -------------------------------------------------------------------------------

bool OfflineTrade::Update(int UserIndex, BYTE Type)
{
	LPOBJ lpUser = &gObj[UserIndex];
	// ----
	if( !lpUser->IsOffTrade )
	{
		return false;
	}
	// ----
	if( Type == 0 )
	{
		lpUser->CheckTick = GetTickCount();
	}
	else if( Type == 1 )
	{
		//offtrade fix (jewel receive)
		if ((lpUser->IsOffTrade) && (!lpUser->m_bPShopOpen))
		{
			return true;
		}
		//fim
		lpUser->ConnectCheckTime = GetTickCount();
	}
	// ----
	return true;
}
// -------------------------------------------------------------------------------

void OfflineTrade::Close(char * AccountID)
{
	for( int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++ )
	{
		LPOBJ lpUser = &gObj[i];
		// ----
		if( lpUser->Connected != PLAYER_PLAYING || !lpUser->IsOffTrade || strcmp(AccountID, lpUser->AccountID) )
		{
			continue;
		}
		// ----
		GJPUserClose(lpUser->AccountID);
		gObjDel(i);
		lpUser->IsOffTrade = false;
		// ----
		g_OfflineTradeLog.Output("[%s] [%s] Offline shop has been closed, account free", 
			lpUser->AccountID, lpUser->Name);
	}
}
// -------------------------------------------------------------------------------

//#ifdef __ALIEN__
bool OfflineTrade::CheckExchange(LPOBJ lpUser, LPOBJ lpTargetUser, int ItemPos)
{

	if(!strcmp(lpUser->m_szPShopText,WCOIN_SHOP))
	{
		if ( lpTargetUser->GameShop.WCoinP < lpUser->Inventory1[ItemPos].m_iPShopValue )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Lack of Wcoin",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more WCoin to buy", lpUser->m_Index, 1);
			return false;
		}
	}
	else if(!strcmp(lpUser->m_szPShopText,SOUL_SHOP))	//Soul check
	{
		if ( this->GetSoulCount(lpTargetUser->m_Index) < lpUser->Inventory1[ItemPos].m_iPShopValue )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Lack of Soul",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Soul to buy", lpTargetUser->m_Index, 1);
			return false;
		}
		else if( !this->EmptyPointInventory(lpUser, lpUser->Inventory1[ItemPos].m_iPShopValue) )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Empty Invintory",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Empty Inventory", lpTargetUser->m_Index, 1);
			return false;
		}
	}
	else if(!strcmp(lpUser->m_szPShopText,BLESS_SHOP))	//Bless check
	{
		if ( this->GetBlessCount(lpTargetUser->m_Index) < lpUser->Inventory1[ItemPos].m_iPShopValue )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Lack of Bless",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Bless to buy", lpTargetUser->m_Index, 1);
			return false;
		}
		else if( !this->EmptyPointInventory(lpUser, lpUser->Inventory1[ItemPos].m_iPShopValue) )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Empty Invintory",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Empty Inventory", lpTargetUser->m_Index, 1);
			return false;
		}
	}
	else if(!strcmp(lpUser->m_szPShopText,CHAOS_SHOP))	//Bless check
	{
		if ( this->GetChaosCount(lpTargetUser->m_Index) < lpUser->Inventory1[ItemPos].m_iPShopValue )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Lack of Chaos",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Chaos to buy", lpTargetUser->m_Index, 1);
			return false;
		}
		else if( !this->EmptyPointInventory(lpUser, lpUser->Inventory1[ItemPos].m_iPShopValue) )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Empty Invintory",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Empty Inventory", lpTargetUser->m_Index, 1);
			return false;
		}
	}
	else if(!strcmp(lpUser->m_szPShopText,LIFE_SHOP))	//Bless check
	{
		if ( this->GetLifeCount(lpTargetUser->m_Index) < lpUser->Inventory1[ItemPos].m_iPShopValue )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Lack of Life",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Life to buy", lpTargetUser->m_Index, 1);
			return false;
		}
		else if( !this->EmptyPointInventory(lpUser, lpUser->Inventory1[ItemPos].m_iPShopValue) )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Empty Invintory",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Empty Inventory", lpTargetUser->m_Index, 1);
			return false;
		}
	}
	else if(!strcmp(lpUser->m_szPShopText,CREATION_SHOP))	//Bless check
	{
		if ( this->GetCreationCount(lpTargetUser->m_Index) < lpUser->Inventory1[ItemPos].m_iPShopValue )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Lack of Creations",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Creation to buy", lpTargetUser->m_Index, 1);
			return false;
		}
		else if( !this->EmptyPointInventory(lpUser, lpUser->Inventory1[ItemPos].m_iPShopValue) )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Empty Invintory",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			GCServerMsgStringSend("Need more Empty Inventory", lpTargetUser->m_Index, 1);
			return false;
		}
	}
	else
	{
		if ( lpTargetUser->Money < lpUser->Inventory1[ItemPos].m_iPShopValue )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Lack of Zen",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			::CGPShopAnsBuyItem(lpTargetUser->m_Index, lpUser->m_Index, 0, 7);
			return false;
		}

		if ( gObjCheckMaxZen(lpUser->m_Index, lpUser->Inventory1[ItemPos].m_iPShopValue) == FALSE )
		{
			LogAddTD("[PShop] [%s][%s] PShop Item Buy Request Failed : [%s][%s] Exceeding Zen of the Host",
				lpTargetUser->AccountID, lpTargetUser->Name, lpUser->AccountID, lpUser->Name);
			::CGPShopAnsBuyItem(lpTargetUser->m_Index, lpUser->m_Index, 0, 8);
			return false;
		}
	} 
	return true;
}
// -------------------------------------------------------------------------------

BOOL OfflineTrade::UpdateExchange(LPOBJ lpUSer, LPOBJ lpTargetUser, int dwCost)
{
	if(!strcmp(lpUSer->m_szPShopText, WCOIN_SHOP))
	{
		lpTargetUser->GameShop.WCoinP -= dwCost;
		lpUSer->GameShop.WCoinP += ceil(dwCost - (dwCost * this->m_wCoinTax));
		gGameShop.GDSavePoint(lpTargetUser->m_Index);
		gGameShop.GDSavePoint(lpUSer->m_Index);
	}
	else if(!strcmp(lpUSer->m_szPShopText, SOUL_SHOP))
	{
		if(!this->DeleteSoulCount(lpTargetUser->m_Index, dwCost))
			return false;
		this->AddedSoul(lpUSer, dwCost);
	}
	else if(!strcmp(lpUSer->m_szPShopText, BLESS_SHOP))
	{
		if(!this->DeleteBlessCount(lpTargetUser->m_Index, dwCost))
			return false;
		this->AddedBless(lpUSer, dwCost);
	}
	else if(!strcmp(lpUSer->m_szPShopText, CHAOS_SHOP))
	{
		if(!this->DeleteChaosCount(lpTargetUser->m_Index, dwCost))
			return false;
		this->AddedChaos(lpUSer, dwCost);
	}
	else if(!strcmp(lpUSer->m_szPShopText, LIFE_SHOP))
	{
		if(!this->DeleteLifeCount(lpTargetUser->m_Index, dwCost))
			return false;
		this->AddedLife(lpUSer, dwCost);
	}
	else if(!strcmp(lpUSer->m_szPShopText, CREATION_SHOP))
	{
		if(!this->DeleteCreationCount(lpTargetUser->m_Index, dwCost))
			return false;
		this->AddedCreation(lpUSer, dwCost);
	}
	else
	{
		lpTargetUser->Money -= dwCost;
		lpUSer->Money += dwCost;
		GCMoneySend(lpTargetUser->m_Index, lpTargetUser->Money);
		GCMoneySend(lpUSer->m_Index, lpUSer->Money);
	}
	return true;
}
// -------------------------------------------------------------------------------

int OfflineTrade::GetSoulCount(int UserIndex)
{
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return -1;
	}
	// ----
	int ItemCount   = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		if( !lpUser->pInventory[i].IsItem() )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[i].m_Type == ITEMGET(12, 31) )
		{
			ItemCount += 10 * (lpUser->pInventory[i].m_Level + 1);
		}
		else if( lpUser->pInventory[i].m_Type == ITEMGET(14, 14) )
		{
			ItemCount++;
		}
	}
	// ----
	return ItemCount;
}
// -------------------------------------------------------------------------------

int OfflineTrade::GetBlessCount(int UserIndex)
{
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return -1;
	}
	// ----
	int ItemCount   = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		if( !lpUser->pInventory[i].IsItem() )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[i].m_Type == ITEMGET(12, 30) )
		{
			ItemCount += 10 * (lpUser->pInventory[i].m_Level + 1);
		}
		else if( lpUser->pInventory[i].m_Type == ITEMGET(14, 13) )
		{
			ItemCount++;
		}
	}
	// ----
	return ItemCount;
}
// -------------------------------------------------------------------------------

int OfflineTrade::GetChaosCount(int UserIndex)
{
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return -1;
	}
	// ----
	int ItemCount   = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		if( !lpUser->pInventory[i].IsItem() )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[i].m_Type == ITEMGET(12, 141) )
		{
			ItemCount += 10 * (lpUser->pInventory[i].m_Level + 1);
		}
		else if( lpUser->pInventory[i].m_Type == ITEMGET(12, 15) )
		{
			ItemCount++;
		}
	}
	// ----
	return ItemCount;
}
// -------------------------------------------------------------------------------

int OfflineTrade::GetLifeCount(int UserIndex)
{
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return -1;
	}
	// ----
	int ItemCount   = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		if( !lpUser->pInventory[i].IsItem() )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[i].m_Type == ITEMGET(12, 136) )
		{
			ItemCount += 10 * (lpUser->pInventory[i].m_Level + 1);
		}
		else if( lpUser->pInventory[i].m_Type == ITEMGET(14, 16) )
		{
			ItemCount++;
		}
	}
	// ----
	return ItemCount;
}
// -------------------------------------------------------------------------------

int OfflineTrade::GetCreationCount(int UserIndex)
{
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return -1;
	}
	// ----
	int ItemCount   = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		if( !lpUser->pInventory[i].IsItem() )
		{
			continue;
		}
		// ----
		if( lpUser->pInventory[i].m_Type == ITEMGET(12, 137) )
		{
			ItemCount += 10 * (lpUser->pInventory[i].m_Level + 1);
		}
		else if( lpUser->pInventory[i].m_Type == ITEMGET(14, 22) )
		{
			ItemCount++;
		}
	}
	// ----
	return ItemCount;
}
// -------------------------------------------------------------------------------

int OfflineTrade::CheckItem(int UserIndex, int ItemID, BYTE ItemLevel)
{
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return -1;
	}
	// ----
	LPOBJ lpUser = &gObj[UserIndex];
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		if( !lpUser->pInventory[i].IsItem() )
		{
			continue;
		}
		// ----
		if(     lpUser->pInventory[i].m_Type == ItemID
			&&  lpUser->pInventory[i].m_Level == ItemLevel )
		{
			return i;
		}
	}
	// ----
	return -1;
}
// -------------------------------------------------------------------------------

bool OfflineTrade::DeleteSoulCount(int UserIndex, int SoulCount)
{
	LogAddTD("[OffTrade][DeBug] SoulCount: %d",SoulCount);
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return false;
	}
	// ----
	if( this->GetSoulCount(UserIndex) < SoulCount )
	{
		return false;
	}
	// ----
	int DeletedItem = 0;
	int CheckedItem = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----CHECK IF SOUL ARE OK BEFORE REMOVING
	while (true)
	{
		int needed = SoulCount - CheckedItem;

		// ----
		if (needed <= 0)
		{
			break;
		}
		// ----
		bool hassoul = FALSE;
		if (needed >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 31), 2) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 31), 2);
			CheckedItem += 30;
			hassoul = TRUE;
		}
		else if (needed >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 31), 1) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 31), 1);
			CheckedItem += 20;
			hassoul = TRUE;
		}
		else if (needed >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 31), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 31), 0);
			CheckedItem += 10;
			hassoul = TRUE;
		}
		else if (this->CheckItem(UserIndex, ITEMGET(14, 14), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(14, 14), 0);
			CheckedItem++;
			hassoul = TRUE;
		}
		if (hassoul == FALSE)
		{
			GCServerMsgStringSend("Please unpack your jewels", lpUser->m_Index, 1);
			return false;
		}
	}
	// ----
	while(true)
	{
		int ItemDiff = SoulCount - DeletedItem;
		// ----
		if( ItemDiff <= 0 )
		{
			break;
		}
		// ----
		if( ItemDiff >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 31), 2) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 31), 2);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 30;
		}
		else if( ItemDiff >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 31), 1) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 31), 1);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 20;
		}
		else if( ItemDiff >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 31), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 31), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 10;
		}
		else if( this->CheckItem(UserIndex, ITEMGET(14, 14), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(14, 14), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem++;
		}
	}
	// ----
	LogAddTD("[OfflineTrade] [%s] [%s] Deleted %d Jewels Of Soul",lpUser->AccountID, lpUser->Name);
	// ----
	return true;
}

bool OfflineTrade::DeleteBlessCount(int UserIndex, int BlessCount)
{
	LogAddTD("[OffTrade][DeBug] BlessCount: %d",BlessCount);
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return false;
	}
	// ----
	if( this->GetBlessCount(UserIndex) < BlessCount )
	{
		return false;
	}
	// ----
	int DeletedItem = 0;
	int CheckedItem = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----CHECK IF SOUL ARE OK BEFORE REMOVING
	while (true)
	{
		int needed = BlessCount - CheckedItem;

		// ----
		if (needed <= 0)
		{
			break;
		}
		// ----
		bool hasJewel = FALSE;
		if (needed >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 30), 2) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 30), 2);
			CheckedItem += 30;
			hasJewel = TRUE;
		}
		else if (needed >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 30), 1) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 30), 1);
			CheckedItem += 20;
			hasJewel = TRUE;
		}
		else if (needed >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 30), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 30), 0);
			CheckedItem += 10;
			hasJewel = TRUE;
		}
		else if (this->CheckItem(UserIndex, ITEMGET(14, 13), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(14, 13), 0);
			CheckedItem++;
			hasJewel = TRUE;
		}
		if (hasJewel == FALSE)
		{
			GCServerMsgStringSend("Please unpack your jewels", lpUser->m_Index, 1);
			return false;
		}
	}
	// ----
	while(true)
	{
		int ItemDiff = BlessCount - DeletedItem;
		// ----
		if( ItemDiff <= 0 )
		{
			break;
		}
		// ----
		if( ItemDiff >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 30), 2) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 30), 2);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 30;
		}
		else if( ItemDiff >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 30), 1) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 30), 1);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 20;
		}
		else if( ItemDiff >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 30), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 30), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 10;
		}
		else if( this->CheckItem(UserIndex, ITEMGET(14, 13), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(14, 13), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem++;
		}
	}
	// ----
	LogAddTD("[OfflineTrade] [%s] [%s] Deleted %d Jewels Of Bless",lpUser->AccountID, lpUser->Name);
	// ----
	return true;
}

bool OfflineTrade::DeleteChaosCount(int UserIndex, int ChaosCount)
{
	LogAddTD("[OffTrade][DeBug] ChaosCount: %d",ChaosCount);
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return false;
	}
	// ----
	if( this->GetChaosCount(UserIndex) < ChaosCount )
	{
		return false;
	}
	// ----
	int DeletedItem = 0;
	int CheckedItem = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----CHECK IF SOUL ARE OK BEFORE REMOVING
	while (true)
	{
		int needed = ChaosCount - CheckedItem;

		// ----
		if (needed <= 0)
		{
			break;
		}
		// ----
		bool hasJewel = FALSE;
		if (needed >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 141), 2) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 141), 2);
			CheckedItem += 30;
			hasJewel = TRUE;
		}
		else if (needed >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 141), 1) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 141), 1);
			CheckedItem += 20;
			hasJewel = TRUE;
		}
		else if (needed >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 141), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 141), 0);
			CheckedItem += 10;
			hasJewel = TRUE;
		}
		else if (this->CheckItem(UserIndex, ITEMGET(12, 15), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(15, 15), 0);
			CheckedItem++;
			hasJewel = TRUE;
		}
		if (hasJewel == FALSE)
		{
			GCServerMsgStringSend("Please unpack your jewels", lpUser->m_Index, 1);
			return false;
		}
	}
	// ----
	while(true)
	{
		int ItemDiff = ChaosCount - DeletedItem;
		// ----
		if( ItemDiff <= 0 )
		{
			break;
		}
		// ----
		if( ItemDiff >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 141), 2) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 141), 2);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 30;
		}
		else if( ItemDiff >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 141), 1) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 141), 1);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 20;
		}
		else if( ItemDiff >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 141), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 141), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 10;
		}
		else if( this->CheckItem(UserIndex, ITEMGET(12, 15), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 15), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem++;
		}
	}
	// ----
	LogAddTD("[OfflineTrade] [%s] [%s] Deleted %d Jewels Of Chaos",lpUser->AccountID, lpUser->Name);
	// ----
	return true;
}

bool OfflineTrade::DeleteLifeCount(int UserIndex, int LifeCount)
{
	LogAddTD("[OffTrade][DeBug] LifeCount: %d",LifeCount);
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return false;
	}
	// ----
	if( this->GetLifeCount(UserIndex) < LifeCount )
	{
		return false;
	}
	// ----
	int DeletedItem = 0;
	int CheckedItem = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----CHECK IF SOUL ARE OK BEFORE REMOVING
	while (true)
	{
		int needed = LifeCount - CheckedItem;

		// ----
		if (needed <= 0)
		{
			break;
		}
		// ----
		bool hasJewel = FALSE;
		if (needed >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 136), 2) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 136), 2);
			CheckedItem += 30;
			hasJewel = TRUE;
		}
		else if (needed >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 136), 1) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 136), 1);
			CheckedItem += 20;
			hasJewel = TRUE;
		}
		else if (needed >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 136), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 136), 0);
			CheckedItem += 10;
			hasJewel = TRUE;
		}
		else if (this->CheckItem(UserIndex, ITEMGET(14, 16), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(14, 16), 0);
			CheckedItem++;
			hasJewel = TRUE;
		}
		if (hasJewel == FALSE)
		{
			GCServerMsgStringSend("Please unpack your jewels", lpUser->m_Index, 1);
			return false;
		}
	}
	// ----
	while(true)
	{
		int ItemDiff = LifeCount - DeletedItem;
		// ----
		if( ItemDiff <= 0 )
		{
			break;
		}
		// ----
		if( ItemDiff >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 136), 2) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 136), 2);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 30;
		}
		else if( ItemDiff >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 136), 1) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 136), 1);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 20;
		}
		else if( ItemDiff >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 136), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 136), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 10;
		}
		else if( this->CheckItem(UserIndex, ITEMGET(14, 16), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(14, 16), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem++;
		}
	}
	// ----
	LogAddTD("[OfflineTrade] [%s] [%s] Deleted %d Jewels Of Life",lpUser->AccountID, lpUser->Name);
	// ----
	return true;
}

bool OfflineTrade::DeleteCreationCount(int UserIndex, int CreationCount)
{
	LogAddTD("[OffTrade][DeBug] CreationCount: %d",CreationCount);
	if( !OBJMAX_RANGE(UserIndex) )
	{
		return false;
	}
	// ----
	if( this->GetCreationCount(UserIndex) < CreationCount )
	{
		return false;
	}
	// ----
	int DeletedItem = 0;
	int CheckedItem = 0;
	LPOBJ lpUser    = &gObj[UserIndex];
	// ----CHECK IF SOUL ARE OK BEFORE REMOVING
	while (true)
	{
		int needed = CreationCount - CheckedItem;

		// ----
		if (needed <= 0)
		{
			break;
		}
		// ----
		bool hasJewel = FALSE;
		if (needed >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 137), 2) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 137), 2);
			CheckedItem += 30;
			hasJewel = TRUE;
		}
		else if (needed >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 137), 1) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 137), 1);
			CheckedItem += 20;
			hasJewel = TRUE;
		}
		else if (needed >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 137), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 137), 0);
			CheckedItem += 10;
			hasJewel = TRUE;
		}
		else if (this->CheckItem(UserIndex, ITEMGET(14, 22), 0) != -1)
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(14, 22), 0);
			CheckedItem++;
			hasJewel = TRUE;
		}
		if (hasJewel == FALSE)
		{
			GCServerMsgStringSend("Please unpack your jewels", lpUser->m_Index, 1);
			return false;
		}
	}
	// ----
	while(true)
	{
		int ItemDiff = CreationCount - DeletedItem;
		// ----
		if( ItemDiff <= 0 )
		{
			break;
		}
		// ----
		if( ItemDiff >= 30 && this->CheckItem(UserIndex, ITEMGET(12, 137), 2) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 137), 2);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 30;
		}
		else if( ItemDiff >= 20 && this->CheckItem(UserIndex, ITEMGET(12, 137), 1) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 137), 1);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 20;
		}
		else if( ItemDiff >= 10 && this->CheckItem(UserIndex, ITEMGET(12, 137), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(12, 137), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem += 10;
		}
		else if( this->CheckItem(UserIndex, ITEMGET(14, 22), 0) != -1 )
		{
			int ItemPos = this->CheckItem(UserIndex, ITEMGET(14, 22), 0);
			gObjInventoryDeleteItem(lpUser->m_Index, ItemPos);
			GCInventoryItemDeleteSend(lpUser->m_Index, ItemPos, 1);
			DeletedItem++;
		}
	}
	// ----
	LogAddTD("[OfflineTrade] [%s] [%s] Deleted %d Jewels Of Creation",lpUser->AccountID, lpUser->Name);
	// ----
	return true;
}



















bool OfflineTrade::EmptyPointInventory(LPOBJ lpUser, int SoulNeed)
{
	int SoulCount = 0;
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		int ItemDiff = SoulNeed - SoulCount;
		// ----
		if( ItemDiff >= 30 && !lpUser->pInventory[i].IsItem() )
		{
			SoulCount += 30;
		}
		else if( ItemDiff >= 20 && !lpUser->pInventory[i].IsItem() )
		{
			SoulCount += 20;
		}
		else if( ItemDiff >= 10 && !lpUser->pInventory[i].IsItem() )
		{
			SoulCount += 10;
		}
		else if( ItemDiff >= 1 && !lpUser->pInventory[i].IsItem() )
		{
			SoulCount++;
		}
		// ----
		if( SoulCount >= SoulNeed || ItemDiff <= 0 )
		{
			break;
		}
	}
	// ----
	if( SoulCount < SoulNeed )
	{
		return false;
	}
	// ----
	return true;
}

void OfflineTrade::AddedSoul(LPOBJ lpUser, int SoulCount)
{
	int Total = 0;
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		int ItemDiff = SoulCount - Total;
		// ----
		if( ItemDiff >= 30 && !lpUser->pInventory[i].IsItem() )
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 31), 2, 255, 0, 0, 0, -1, 0, 0);
			Total += 30;
		}
		else if( ItemDiff >= 20 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 31), 1, 255, 0, 0, 0, -1, 0, 0);
			Total += 20;
		}
		else if( ItemDiff >= 10 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 31), 0, 255, 0, 0, 0, -1, 0, 0);
			Total += 10;
		}
		else if( ItemDiff >= 1 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(14, 14), 0, 255, 0, 0, 0, -1, 0, 0);
			Total++;
		}
		if( SoulCount == Total ) return;
	}
	// ----

}

void OfflineTrade::AddedBless(LPOBJ lpUser, int BlessCount)
{
	int Total = 0;
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		int ItemDiff = BlessCount - Total;
		// ----
		if( ItemDiff >= 30 && !lpUser->pInventory[i].IsItem() )
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 30), 2, 255, 0, 0, 0, -1, 0, 0);
			Total += 30;
		}
		else if( ItemDiff >= 20 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 30), 1, 255, 0, 0, 0, -1, 0, 0);
			Total += 20;
		}
		else if( ItemDiff >= 10 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 30), 0, 255, 0, 0, 0, -1, 0, 0);
			Total += 10;
		}
		else if( ItemDiff >= 1 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(14, 13), 0, 255, 0, 0, 0, -1, 0, 0);
			Total++;
		}
		if( BlessCount == Total ) return;
	}
	// ----

}

void OfflineTrade::AddedChaos(LPOBJ lpUser, int ChaosCount)
{
	int Total = 0;
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		int ItemDiff = ChaosCount - Total;
		// ----
		if( ItemDiff >= 30 && !lpUser->pInventory[i].IsItem() )
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 141), 2, 255, 0, 0, 0, -1, 0, 0);
			Total += 30;
		}
		else if( ItemDiff >= 20 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 141), 1, 255, 0, 0, 0, -1, 0, 0);
			Total += 20;
		}
		else if( ItemDiff >= 10 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 141), 0, 255, 0, 0, 0, -1, 0, 0);
			Total += 10;
		}
		else if( ItemDiff >= 1 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 15), 0, 255, 0, 0, 0, -1, 0, 0);
			Total++;
		}
		if( ChaosCount == Total ) return;
	}
	// ----

}

void OfflineTrade::AddedLife(LPOBJ lpUser, int LifeCount)
{
	int Total = 0;
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		int ItemDiff = LifeCount - Total;
		// ----
		if( ItemDiff >= 30 && !lpUser->pInventory[i].IsItem() )
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 136), 2, 255, 0, 0, 0, -1, 0, 0);
			Total += 30;
		}
		else if( ItemDiff >= 20 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 136), 1, 255, 0, 0, 0, -1, 0, 0);
			Total += 20;
		}
		else if( ItemDiff >= 10 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 136), 0, 255, 0, 0, 0, -1, 0, 0);
			Total += 10;
		}
		else if( ItemDiff >= 1 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(14, 16), 0, 255, 0, 0, 0, -1, 0, 0);
			Total++;
		}
		if( LifeCount == Total ) return;
	}
	// ----

}

void OfflineTrade::AddedCreation(LPOBJ lpUser, int CreationCount)
{
	int Total = 0;
	// ----
	for( int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++ )
	{
		int ItemDiff = CreationCount - Total;
		// ----
		if( ItemDiff >= 30 && !lpUser->pInventory[i].IsItem() )
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 137), 2, 255, 0, 0, 0, -1, 0, 0);
			Total += 30;
		}
		else if( ItemDiff >= 20 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 137), 1, 255, 0, 0, 0, -1, 0, 0);
			Total += 20;
		}
		else if( ItemDiff >= 10 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(12, 137), 0, 255, 0, 0, 0, -1, 0, 0);
			Total += 10;
		}
		else if( ItemDiff >= 1 && !lpUser->pInventory[i].IsItem())
		{
			ItemSerialCreateSend(lpUser->m_Index, 235, 0, 0, ITEMGET(14, 22), 0, 255, 0, 0, 0, -1, 0, 0);
			Total++;
		}
		if( CreationCount == Total ) return;
	}
	// ----

}
//#endif