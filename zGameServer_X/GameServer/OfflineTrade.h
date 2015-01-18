#pragma once
// -------------------------------------------------------------------------------

#include "MapClass.h"
#include "StdAfx.h"
//#ifdef __ALIEN__
#define ZEN_SHOP	".::ZEN SHOP::."
#define WCOIN_SHOP	".::WCOIN SHOP::."
#define SOUL_SHOP	".::SOUL SHOP::."
#define BLESS_SHOP	".::BLESS SHOP::."
#define CHAOS_SHOP	".::CHAOS SHOP::."
#define LIFE_SHOP	".::LIFE SHOP::."
#define CREATION_SHOP	".::CREATION SHOP::."
//#endif
// -------------------------------------------------------------------------------

class OfflineTrade
{
public:
			OfflineTrade();
			~OfflineTrade();
	// ----
	void	Init();
	void	Load();
	void	ReadData(char * File);
	// ----
	bool	CheckReq(int UserIndex);
//#ifdef __ALIEN__
	void	Start(int UserIndex, int Type);
//#else
//	void	Start(int UserIndex);
//#endif
	bool	Update(int UserIndex, BYTE Type);
	void	Close(char * AccountID);
//#ifdef __ALIEN__
	bool	CheckExchange(LPOBJ lpUser, LPOBJ lpTargetUser, int ItemPos);
	BOOL	UpdateExchange(LPOBJ lpUSer, LPOBJ lpTargetUser, int dwCost);
	int		GetSoulCount(int UserIndex);
	int		GetBlessCount(int UserIndex);
	int		GetChaosCount(int UserIndex);
	int		GetLifeCount(int UserIndex);
	int		GetCreationCount(int UserIndex);
	int		CheckItem(int UserIndex, int ItemID, BYTE ItemLevel);
	bool	DeleteSoulCount(int UserIndex, int SoulCount);
	bool	DeleteBlessCount(int UserIndex, int BlessCount);
	bool	DeleteChaosCount(int UserIndex, int ChaosCount);
	bool	DeleteLifeCount(int UserIndex, int LifeCount);
	bool	DeleteCreationCount(int UserIndex, int CreationCount);

	bool	EmptyPointInventory(LPOBJ lpUser, int NeedPoint);
	void	AddedSoul(LPOBJ lpUSer, int SoulCount);
	void	AddedBless(LPOBJ lpUSer, int BlessCount);
	void	AddedChaos(LPOBJ lpUSer, int ChaosCount);
	void	AddedLife(LPOBJ lpUSer, int LifeCount);
	void	AddedCreation(LPOBJ lpUSer, int CreationCount);
//#endif
	// ----
private:
	WORD	m_ReqLevel;
	WORD	m_ReqReset;
	bool	m_UseMapList;
	DWORD	m_Cost[4];
	BYTE	m_MapList[MAX_NUMBER_MAP];
	BYTE	m_LoadedMapList;
	float	m_wCoinTax;
	// ----

//#ifdef __ALIEN__
public:
	bool EnableZen;
	bool EnableWCoin;
	bool EnableSoul;
	bool EnableBless;
	bool EnableChaos;
	bool EnableLife;
	bool EnableCreation;
//#endif
}; extern OfflineTrade g_OfflineTrade;
// -------------------------------------------------------------------------------