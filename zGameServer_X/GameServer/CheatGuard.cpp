#include "StdAfx.h"
#include "CheatGuard.h"
#include "user.h"
#include "logproc.h"
#include "GameMain.h"
#include "..\include\ReadScript.h"
//#if defined __BEREZNUK__ || __MIX__ || __REEDLAN__ || __MUANGEL__ || __WHITE__ || __MEGAMU__ || __VIRNET__
#include "ConnectEx.h"
//#endif
// -------------------------------------------------------------------------------

CheatGuard g_CheatGuard;
// -------------------------------------------------------------------------------

CheatGuard::CheatGuard()
{
	this->Init();
}
// -------------------------------------------------------------------------------

CheatGuard::~CheatGuard()
{

}
// -------------------------------------------------------------------------------

void CheatGuard::Init()
{
	ZeroMemory(this->XOR, sizeof(this->XOR));
	this->m_UseDisconnect		= true;
	this->m_PacketsPerSecond	= 100;
	this->m_SpeedHackCheck		= false;
}
// -------------------------------------------------------------------------------

void CheatGuard::ReadXOR(char * File)
{
	int Token;
	this->Init();
	// ----
	SMDFile		= fopen(File, "r");
	int Count	= 0;
	// ----
	if( SMDFile == 0 )
	{
		MsgBox(lMsg.Get(MSGGET(0, 112)), File);
		return;
	}
	// ----
	while(true) 
	{
		Token = GetToken();
		// ----
		if( Token == END )
		{
			break;
		}
		else
		{
			this->XOR[Count] = TokenNumber;
			Count++;
		}
	}
	// ----
	LogAddTD("[CheatGuard] [%d] Keys loaded", Count);
	fclose(SMDFile);
}
// -------------------------------------------------------------------------------

void CheatGuard::ReadMain(char * File)
{
	this->m_UseDisconnect		= GetPrivateProfileInt("Common", "UseDisconnect", true, File);
	this->m_PacketsPerSecond	= GetPrivateProfileInt("Common", "MaxPacketPerSecond", 120, File);
	this->m_SpeedHackCheck		= GetPrivateProfileInt("Common", "SpeedHackCheck", true, File);
}
// -------------------------------------------------------------------------------

void CheatGuard::Load()
{
	this->ReadXOR(gDirPath.GetNewPath("CheatGuard\\ProtocolFilter.dat"));
	this->ReadMain(gDirPath.GetNewPath("CheatGuard\\CheatGuard.ini"));
}
// -------------------------------------------------------------------------------

void CheatGuard::Disconnect(int aIndex)
{
	if( this->m_UseDisconnect )
	{
		//#if defined __BEREZNUK__ || __MIX__ || __REEDLAN__ || __MUANGEL__ || __WHITE__ || __MEGAMU__ || __VIRNET__
		if(g_AutoReconnect == 1)
		{
			g_ConnectEx.SendClose(aIndex);
		}
		//#endif
		CloseClient(aIndex);
	}
}
// -------------------------------------------------------------------------------

void CheatGuard::GenerateFileName(char * Buff)
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	// ----
	sprintf(Buff, "LOG\\CheatGuard\\%02d_%02d_%02d.log", t.wDay, t.wMonth, t.wYear);
}
// -------------------------------------------------------------------------------

void CheatGuard::WriteLog(char * Text, ...)
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	// ----
	char Buff[300];
	char Message[300];
	char FileName[40];
	this->GenerateFileName(FileName);
	FILE * Log = fopen(FileName, "a+");	//ok
	// ----
	va_list pArguments;
	va_start(pArguments, Text);
	vsprintf_s(Message, Text, pArguments);
	va_end(pArguments);
	// ----
	sprintf_s(Buff, "[%02d:%02d:%02d] %s\n", t.wHour, t.wMinute, t.wSecond, Message);
	// ----
	LogAddC(2, " [CheatGuard] %s", Message);
	fprintf(Log, "%s", Buff);
	// ----
	fclose(Log);
}
// -------------------------------------------------------------------------------

void CheatGuard::GetCheckResult(CHEATGUARD_ANS_CHECK * pResult, int aIndex)
{
	if( gObj[aIndex].CloseType == 1 )
	{
		return;
	}
	// ----
	char * MemberType[] = { "Level", "LevelPoint", "Strength", "Dexterity", 
		"Vitality", "Energy", "Leadership", "AttackSpeed", "MagicSpeed", "FrameValue"};
	// ----
	LPOBJ lpUser		= &gObj[aIndex];
	bool Disconnect		= false;
	int OriginalValue	= 0;
	// ----
	pResult->Data[emLevelPoint].Status	= true;	//Simple its not need
	pResult->Data[emLevel].Status		= true;	//Simple its not need
	pResult->Data[emEnergy].Status		= true;	//Simple its not need
	pResult->Data[emVitality].Status	= true;	//Simple its not need
	pResult->Data[emStrength].Status	= true;	//Simple its not need
	pResult->Data[emLeadership].Status	= true;	//Simple its not need
	// ----
	for( int i = 0; i < MAX_CHECK_VARS; i++ )
	{
		if( !pResult->Data[i].Status )
		{
			Disconnect = true;
			// ----
			switch(i)
			{
			case emLevel:
				{
					OriginalValue = lpUser->Level;
				}
				break;
				// --
			case emLevelPoint:
				{
					OriginalValue = lpUser->LevelUpPoint;
				}
				break;
				// --
			case emStrength:
				{
					OriginalValue = lpUser->Strength;
				}
				break;
				// --
			case emDexterity:
				{
					OriginalValue = lpUser->Dexterity;
				}
				break;
				// --
			case emVitality:
				{
					OriginalValue = lpUser->Vitality;
				}
				break;
				// --
			case emEnergy:
				{
					OriginalValue = lpUser->Energy;
				}
				break;
				// --
			case emLeadership:
				{
					OriginalValue = lpUser->Leadership;
				}
				break;
				// --
			case emAttackSpeed:
				{
					OriginalValue = lpUser->m_AttackSpeed;
				}
				break;
				// --
			case emMagicSpeed:
				{
					OriginalValue = lpUser->m_MagicSpeed;
				}
				break;
			}
			// ----
			this->WriteLog("[%s][%s] [%s] %d | %d", gObj[aIndex].AccountID, gObj[aIndex].Name, MemberType[i], OriginalValue, pResult->Data[i].Value);
		}
	}
	// ----
	if( Disconnect )
	{
		this->Disconnect(aIndex);
	}
}
// -------------------------------------------------------------------------------

void CheatGuard::SendCheckRequest(int aIndex)
{
	CHEATGUARD_REQ_CHECK pRequest;
	pRequest.Head.set((LPBYTE)&pRequest, 0x76, sizeof(pRequest));
	LPOBJ lpUser = &gObj[aIndex];
	// ----
	pRequest.Level			= lpUser->Level;
	pRequest.LevelPoint		= lpUser->LevelUpPoint;
	pRequest.Strength		= lpUser->Strength;
	pRequest.Dexterity		= lpUser->Dexterity;
	pRequest.Vitality		= lpUser->Vitality;
	pRequest.Energy			= lpUser->Energy;
	pRequest.Leadership		= lpUser->Leadership;
	pRequest.AttackSpeed	= lpUser->m_AttackSpeed;
	pRequest.MagicSpeed		= lpUser->m_MagicSpeed;
	// ----
	if( gObjIsGamePlaing(lpUser) )
	{
		DataSend(aIndex, (LPBYTE)&pRequest, sizeof(pRequest));
	}
}
// -------------------------------------------------------------------------------

configs cfg;
DWORD WINAPI LoadGXProtectServer(LPVOID p)
{
	int err, on, l, b;
	int bytesRecv;
	char recvbuf[100];
	char banforever = '0';
	char buf[100];

	cfg.wVersionRequested = MAKEWORD(1,1);
	err = WSAStartup(cfg.wVersionRequested, &cfg.wsaData);

	if(err != 0)
	{
		printf("WSAStartup error %ld", WSAGetLastError());
		WSACleanup();
		return false; //error connect
	}

	memset(&cfg.channel, 0, sizeof(cfg.channel));
	cfg.channel.sin_family = AF_INET;
	cfg.channel.sin_addr.s_addr = htonl(INADDR_ANY);
	cfg.channel.sin_port = htons(55696);

	cfg.s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(cfg.s < 0)
	{
		printf("Socket Error %ld", WSAGetLastError());
		WSACleanup();
		return false ;
	}
	setsockopt(cfg.s, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

	b = bind(cfg.s,(struct sockaddr*)&cfg.channel, sizeof(cfg.channel));
	if(b < 0)
	{
		printf("Bind Error %ld",WSAGetLastError());
		WSACleanup();
		return  false;
	}

	l = listen(cfg.s,100);
	if(l < 0)
	{
		printf("Listen Error %ld", WSAGetLastError());
		WSACleanup();
		return false;
	}

	while(true)
	{
		cfg.sa = accept(cfg.s, 0, 0);
		if(cfg.sa < 0)
		{
			printf("Accept Error %ld", WSAGetLastError());
			WSACleanup();
			return false;
		}
		bytesRecv = recv(cfg.sa, recvbuf, 101, 0);
		err = WSAGetLastError();
		if(bytesRecv == 0 || bytesRecv == WSAECONNRESET)
		{
			printf("Connection Closed.");
			WSACleanup();
		}

		char compareSend[100];
		bool isEqual = false;

		wsprintf(compareSend+1,"protectSend");

		for (int i = 0; i < 100; i++)
		{
			if(compareSend[i] == recvbuf[i])
				isEqual = true;
			else
			{
				isEqual = false;
				break;
			}
		}

		if(isEqual)
		{
			g_GXProtectConnection = TRUE;
		}
		else
		{
			g_GXProtectConnection = FALSE;
		}

		//buf[0] = '1';
		//bytesRecv = send(cfg.sa,buf,100,0);
		closesocket(cfg.sa);
	}
	closesocket(cfg.s);
	WSACleanup();
}