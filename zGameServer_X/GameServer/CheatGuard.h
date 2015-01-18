#pragma once
// -------------------------------------------------------------------------------

#define MAX_CHECK_VARS		10
// -------------------------------------------------------------------------------

#include "..\include\prodef.h"
// -------------------------------------------------------------------------------

enum eCheckStatMember
{
	emLevel			= 0,
	emLevelPoint	= 1,
	emStrength		= 2,
	emDexterity		= 3,
	emVitality		= 4,
	emEnergy		= 5,
	emLeadership	= 6,
	emAttackSpeed	= 7,
	emMagicSpeed	= 8,
	emFrameValue	= 9,
};
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct CHEATGUARD_REQ_CHECK
{
	PBMSG_HEAD	Head;
	// ----
	WORD		Level;
	int			LevelPoint;
	WORD		Strength;
	WORD		Dexterity;
	WORD		Vitality;
	WORD		Energy;
	WORD		Leadership;
	int			AttackSpeed;
	int			MagicSpeed;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct CHEATGUARD_ANS_DATA
{
	bool		Status;
	int			Value;
};
#pragma pack(pop)
// -------------------------------------------------------------------------------

#pragma pack(push, 1)
struct CHEATGUARD_ANS_CHECK
{
	PBMSG_HEAD	Head;
	// ----
	CHEATGUARD_ANS_DATA Data[MAX_CHECK_VARS];
};
#pragma pack(pop)
// -------------------------------------------------------------------------------


class CheatGuard
{
public:
			CheatGuard();
			~CheatGuard();
	// ----
	void	Init();
	void	ReadXOR(char * File);
	void	ReadMain(char * File);
	void	Load();
	// ----
	void	Disconnect(int aIndex);
	void	GenerateFileName(char * Buff);
	void	WriteLog(char * Text, ...);
	void	GetCheckResult(CHEATGUARD_ANS_CHECK * pResult, int aIndex);
	void	SendCheckRequest(int aIndex);
	// ----
	BYTE	XOR[32];
	// ----
public:
	bool	m_UseDisconnect;
	int		m_PacketsPerSecond;
	bool	m_SpeedHackCheck;
	// ----
}; extern CheatGuard g_CheatGuard;
// -------------------------------------------------------------------------------

DWORD	WINAPI LoadGXProtectServer(LPVOID p);

struct configs
{
	char cBanTime[500];
	char cVersion[10];
	char cFuncWordBan[5];
	char cLog[5];
	char cLogAll[5];

	int AutoBanTime;
	int didbanname;
	int didlogtxt;
	int didlogonscr;
	int MacProtect;
	char ServerVer[3];
	char ServerVers[3];

	SOCKET s;
	SOCKET sa;
	struct sockaddr_in channel;
	WORD wVersionRequested;
	WSADATA wsaData;
	unsigned char MACData[6];
	char HostNameData[20];
	char RemoteIP[17];
	char account[11];
	char ban_status;
	char User_Name[20];
	char add_ban;
	char BanReason[30];
	SYSTEMTIME st;
}; extern configs;