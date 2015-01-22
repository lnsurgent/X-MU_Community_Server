#include "stdafx.h"


#include "SocketItem.h"
#include "ReadScript.h"
#include "TMemory.h"
#ifdef __LOL__
cSocketItem gSocketItem;

void cSocketItem::Load()
{
	SMDFile = fopen("Data\\Custom\\SocketItem.ini", "r");

	if ( SMDFile == NULL )
	{
		MessageBoxA(0,"Data\\Custom\\SocketItem.ini","CRITICAL ERROR",0);
		ExitProcess(1);
		return;
	}

	int Token;
	int type = -1;

	this->ItemCount = 0;

	//while ( true )
	//{
	//    Token = GetToken();

 //       if( Token == 2)
	//	{
 //           break;
	//	}

	//	type = (int)TokenNumber;

		while ( true )
		{
			//if ( type < 0 || type > 4 )
			//{
			//	break;
			//}

		    Token = GetToken();

			if ( strcmp("end", TokenString ) == 0)
			{
				break;
			}

			this->sItem[this->ItemCount].Type = TokenNumber;

			Token = GetToken();
			this->sItem[this->ItemCount].Index = TokenNumber;

			this->ItemCount++;
		}
    //}

	fclose(SMDFile);

	SetOp((LPVOID)0x00967FC1, gSocketItem.CheckItem, ASM::CALL);
	SetOp((LPVOID)0x00967FE7, gSocketItem.CheckItem, ASM::CALL);
}

int cSocketItem::CheckItem(signed int ItemCode)
{
	for(int i = 0; i < gSocketItem.ItemCount; i++ )
	{
		if( ItemCode == ITEMGET(gSocketItem.sItem[i].Type, gSocketItem.sItem[i].Index) )
		{
			return 1;
		}
		else if( gSocketItem.sItem[i].Type == -1 )
		{
			for( int Category = 7; Category < 12; Category++ )
			{
				if( ItemCode == ITEMGET(Category, gSocketItem.sItem[i].Index) )
				{
					return 1;
				}
			}
		}
	}
	return 0;
}
#endif