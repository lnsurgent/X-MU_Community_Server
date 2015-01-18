#include "stdafx.h"
#include "CheatGuard.h"
#include "Import.h"
#include "TMemory.h"
#include "User.h"
#include "Protocol.h"
#include "TrayMode.h"
// ----------------------------------------------------------------------------------------------

char recvbuf[100];
char buf[100];

CheatGuard gCheatGuard;
// ----------------------------------------------------------------------------------------------

void CheatGuard::Load()
{
	// ----
	ZeroMemory(this->XOR, sizeof(XOR));
	this->ChangeXORFilter();
	// ----
}
// ----------------------------------------------------------------------------------------------

void CheatGuard::ChangeXORFilter()
{
	// ----
	this->XOR[0]	= 0xfa;	this->XOR[1]	= 0x5a;	this->XOR[2]	= 0xcc;	this->XOR[3]	= 0x8a;	this->XOR[4]	= 0xf6;	this->XOR[5]	= 0x7a;	this->XOR[6]	= 0x9d;	this->XOR[7]	= 0x31;	this->XOR[8]	= 0x92;	this->XOR[9]	= 0x44;	this->XOR[10]	= 0xaa;	this->XOR[11]	= 0xae;	this->XOR[12]	= 0xaf;	this->XOR[13]	= 0xff; 	this->XOR[14]	= 0xfe;	this->XOR[15]	= 0xbb;	this->XOR[16]	= 0xab;	this->XOR[17]	= 0xba;	this->XOR[18]	= 0xb6;	this->XOR[19]	= 0xd;	this->XOR[20]	= 0x4;	this->XOR[21]	= 0x16;	this->XOR[22]	= 0x4d;	this->XOR[23]	= 0xae;	this->XOR[24]	= 0xec;	this->XOR[25]	= 0x7f;	this->XOR[26]	= 0xdf;	this->XOR[27]	= 0x1c;	this->XOR[28]	= 0x31;	this->XOR[29]	= 0xba;	this->XOR[30]	= 0xab;	this->XOR[31]	= 0x6a;
	// ----
	for( int i = 0; i < 32; i++ )
	{
		SetByte((PVOID)((oXORFilterStart + 4 * i) + 3), this->XOR[i]);
	}
	// ----
}
// ----------------------------------------------------------------------------------------------

bool CheatGuard::IsCorrectFrame()
{
	if(		pFrameValue1 > MAX_FRAME_VALUE
		||	pFrameValue2 > MAX_FRAME_VALUE
		||	pFrameValue3 > MAX_FRAME_VALUE
		||	pFrameValue4 > MAX_FRAME_VALUE )
	{
		return false;
	}
	// ----
	return true;
}
// ----------------------------------------------------------------------------------------------

int CheatGuard::GetLargerFrame()
{
	if( pFrameValue1 > MAX_FRAME_VALUE )
	{
		return pFrameValue1;
	}
	else if( pFrameValue2 > MAX_FRAME_VALUE )
	{
		return pFrameValue2;
	}
	else if( pFrameValue3 > MAX_FRAME_VALUE )
	{
		return pFrameValue3;
	}
	else if( pFrameValue4 > MAX_FRAME_VALUE )
	{
		return pFrameValue4;
	}
	// ----
	return 0;
}
// ----------------------------------------------------------------------------------------------

void CheatGuard::Check(CHEATGUARD_REQ_CHECK * pRequest)
{
	// ----
	gObjUser.Refresh();
	// ----
	CHEATGUARD_ANS_CHECK pResult;
	pResult.Head.set((LPBYTE)&pResult, 0x76, sizeof(pResult));
	ZeroMemory(pResult.Data, sizeof(pResult.Data));
	// ----
	for( int i = 0; i < MAX_CHECK_VARS; i++ )
	{
		pResult.Data[i].Status = true;
	}
	// ----
	if( gObjUser.lpPlayer->Level != pRequest->Level )
	{
		pResult.Data[emLevel].Status		= false;
		pResult.Data[emLevel].Value			= gObjUser.lpPlayer->Level;
	}
	// ----
	if( gObjUser.lpPlayer->LevelPoint != pRequest->LevelPoint )
	{
		pResult.Data[emLevelPoint].Status	= false;
		pResult.Data[emLevelPoint].Value	= gObjUser.lpPlayer->LevelPoint;
	}
	// ----
	if( gObjUser.lpPlayer->Strength > pRequest->Strength + MAX_STAT_LIMIT 
		|| pRequest->Strength > gObjUser.lpPlayer->Strength + MAX_STAT_LIMIT )
	{
		pResult.Data[emStrength].Status		= false;
		pResult.Data[emStrength].Value		= gObjUser.lpPlayer->Strength;
	}
	// ----
	if( gObjUser.lpPlayer->Dexterity > pRequest->Dexterity + MAX_STAT_LIMIT
		|| pRequest->Dexterity > gObjUser.lpPlayer->Dexterity + MAX_STAT_LIMIT )
	{
		pResult.Data[emDexterity].Status	= false;
		pResult.Data[emDexterity].Value		= gObjUser.lpPlayer->Dexterity;
	}
	// ----
	if( gObjUser.lpPlayer->Vitality > pRequest->Vitality + MAX_STAT_LIMIT
		|| pRequest->Vitality > gObjUser.lpPlayer->Vitality + MAX_STAT_LIMIT )
	{
		pResult.Data[emVitality].Status		= false;
		pResult.Data[emVitality].Value		= gObjUser.lpPlayer->Vitality;
	}
	// ----
	if( gObjUser.lpPlayer->Energy > pRequest->Energy + MAX_STAT_LIMIT
		|| pRequest->Energy > gObjUser.lpPlayer->Energy + MAX_STAT_LIMIT )
	{
		pResult.Data[emEnergy].Status		= false;
		pResult.Data[emEnergy].Value		= gObjUser.lpPlayer->Energy;
	}
	// ----
	if( gObjUser.lpPlayer->Leadership > pRequest->Leadership + MAX_STAT_LIMIT
		|| pRequest->Leadership > gObjUser.lpPlayer->Leadership + MAX_STAT_LIMIT )
	{
		pResult.Data[emLeadership].Status	= false;
		pResult.Data[emLeadership].Value	= gObjUser.lpPlayer->Leadership;
	}
	// ----
	if( (gObjUser.lpPlayer->AttackSpeed > pRequest->AttackSpeed + MAX_SPEED_LIMIT 
		|| pRequest->AttackSpeed > gObjUser.lpPlayer->AttackSpeed + MAX_SPEED_LIMIT) 
		&& (gObjUser.lpPlayer->AttackSpeed > pRequest->MagicSpeed + MAX_SPEED_LIMIT
		|| pRequest->MagicSpeed > gObjUser.lpPlayer->AttackSpeed + MAX_SPEED_LIMIT) )
	{
		pResult.Data[emAttackSpeed].Status	= false;
		pResult.Data[emAttackSpeed].Value	= gObjUser.lpPlayer->AttackSpeed;
		pResult.Data[emMagicSpeed].Value	= gObjUser.lpPlayer->MagicSpeed;
	}
	// ----
	if( !IsCorrectFrame() && !gTrayMode.InTray )
	{
		pResult.Data[emFrameValue].Status	= false;
		pResult.Data[emFrameValue].Value	= this->GetLargerFrame();
	}
	// ----
	for( int i = 0; i < MAX_CHECK_VARS; i++ )
	{
		if( !pResult.Data[i].Status )
		{
			gProtocol.DataSend((LPBYTE)&pResult, pResult.Head.size);
			return;
		}
	}
	// ----
}
// ----------------------------------------------------------------------------------------------

void ConnectToServer(void)
{
	int ConnectDone;
	ConnectDone = ServerConnect();
}

int ServerConnect ()
{
	WORD		wVersionRequested;
	WSADATA		wsaData;
	SOCKADDR_IN target; //Socket address information
	SOCKET		s;
	int			err;
	int			bytesSent;
	int			TryToConnect;
	char*		ptTime;
	char		tTime[12];


	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) {
		//printf("WSAStartup error %ld", WSAGetLastError() );
		WSACleanup();
		return false;
	}
	//------------------------------------------------------

	//---- Build address structure to bind to socket.--------  
	target.sin_family = AF_INET; // address family Internet
	target.sin_port = htons (55696); //Port to connect on
	target.sin_addr.s_addr = inet_addr ("192.168.1.100"); //Target IP
	//--------------------------------------------------------


	// ---- create SOCKET--------------------------------------
	s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
	if (s == INVALID_SOCKET)
	{
		//printf("socket error %ld" , WSAGetLastError() );
		WSACleanup();
		return false; //Couldn't create the socket
	}  
	//---------------------------------------------------------


	//---- try CONNECT -----------------------------------------
	for(TryToConnect=0;TryToConnect<=10;TryToConnect++)
	{
		if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
		{

			//printf("connect error %ld", WSAGetLastError() );
			//printf("attempt to connect #%d to Anti-Hack Server \n",TryToConnect+1);
			Sleep(500);
			if (TryToConnect == 10)
			{
				WSACleanup();
				return false; //Couldn't connect
			}
		}
		else
			break;
	}
	//-----------------------------------------------------------

	//---- SEND bytes -------------------------------------------

	wsprintf(buf+1, "protectSend");
	bytesSent = send( s, buf, 100, 0 ); 
	
	closesocket( s );
	WSACleanup();
	return (0);
}