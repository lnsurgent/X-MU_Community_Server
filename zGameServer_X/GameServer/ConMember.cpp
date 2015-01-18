//	GS-CS	1.00.90	JPN	-	Completed
#include "stdafx.h"
#include "ConMember.h"
#include "logproc.h"
#include "GameMain.h"
#include "..\include\readscript.h"
#include <fstream>
#include "DSProtocol.h"
#include "DBSockMng.h" 

CConMember ConMember;

CConMember::CConMember()
{
	return;
}

CConMember::~CConMember()
{
	return;
}

void CConMember::Init()
{
	this->m_szAccount.clear();
}

BOOL CConMember::IsMember(char * AccountID )
{
	if ( (AccountID == NULL ) || ( strcmp(AccountID, "") == 0 ) )
	{
		return FALSE;
	}

	std::map<std::string, int>::iterator it = this->m_szAccount.find( (std::string) AccountID );

	if ( it != this->m_szAccount.end() )
	{
		return TRUE;
	}

	return FALSE;
}

void CConMember::Load(char* filename)
{
	int count=0;
	int Token;

	this->Init();

	SMDFile = fopen( filename, "r");	//ok

	if ( SMDFile == 0 )
	{
		MsgBox( lMsg.Get( MSGGET( 0, 112 ) ), filename );
		return;
	}

	int n = 0;

	while ( true ) 
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}
		else
		{
			if ( Token == 0 )
			{
				this->m_szAccount.insert( std::pair<std::string, int>( (std::string)TokenString, 0 ) );
			}
		}
	}

	fclose( SMDFile );
}

void CConMember::InsertUser(char * AccountId)
{
	if(!this->IsMember(AccountId))
	{
		char g_ConnectMemberFile[300];
		//VIP SYSTEM
		GetPrivateProfileStringA("GameServerInfo", "ConnectMemberFileLocation","..\\VipSystem\\ConnectMember.txt", g_ConnectMemberFile, 300, SERVER_INFO_PATH);
		fstream datafile(g_ConnectMemberFile, fstream::in | fstream::out | fstream::app);
		if(datafile)
		{
			char retorno[25];
			sprintf(retorno, "\n\"%s\"", AccountId);
			datafile << retorno;
		}
		datafile.close();
	}	
}

void CConMember::UpdateVipStatus()
{
	UPDATEVIPSTATUS pReq;
	pReq.h.set((LPBYTE)&pReq, 0xD7, 0x02, sizeof(UPDATEGOLDCHANNELSTATUS));
	pReq.Duration = 1; //1 min
	cDBSMng.Send((char*)&pReq, sizeof(pReq));
}

void CConMember::RemoveVipStatus()
{
	REMOVEVIPSTATUS pReq;
	pReq.h.set((LPBYTE)&pReq, 0xD7, 0x03, sizeof(REMOVEVIPSTATUS));
	pReq.Value = 0; // set 0 to vip
	cDBSMng.Send((char*)&pReq, sizeof(pReq));
}

void CConMember::CheckVipStatus(char * AccountId, int aIndex)
{
	CHECKVIP pReq;
	pReq.h.set((LPBYTE)&pReq, 0xD7, 0x05, sizeof(CHECKVIP));
	memcpy(pReq.AccountId, AccountId, 10);
	pReq.aIndex = aIndex;
	cDBSMng.Send((char*)&pReq, sizeof(pReq)); 
}