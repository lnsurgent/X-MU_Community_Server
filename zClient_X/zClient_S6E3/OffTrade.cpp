#include "stdafx.h"
#include "OffTrade.h"
#include "TMemory.h"

cOffTrade gOffTrade;

void cOffTrade::Init()
{
	this->TypeShop = 0;
	SetOp((LPVOID)0x007E44F1, (LPVOID)this->ShowPrice, ASM::CALL);
	SetOp((LPVOID)0x007E4603, (LPVOID)this->ShowTwoString, ASM::CALL);
}

void cOffTrade::RecvPShop(PMSG_ANS_BUYLIST_FROM_PSHOP *Data)
{
	if(!strcmp(Data->szPShopText,WCOIN_SHOP))
	{
		this->TypeShop = 1;
	}
	else if(!strcmp(Data->szPShopText,SOUL_SHOP))
	{
		this->TypeShop = 2;
	}
	else if(!strcmp(Data->szPShopText,BLESS_SHOP))
	{
		this->TypeShop = 3;
	}
	else if(!strcmp(Data->szPShopText,CHAOS_SHOP))
	{
		this->TypeShop = 4;
	}
	else if(!strcmp(Data->szPShopText,LIFE_SHOP))
	{
		this->TypeShop = 5;
	}
	else if(!strcmp(Data->szPShopText,CREATION_SHOP))
	{
		this->TypeShop = 6;
	}
	else
	{
		this->TypeShop = 0;
	}
}

void cOffTrade::ShowPrice(DWORD a1, const char *a2, char *Price)
{
	if(gOffTrade.TypeShop == 1)
	{
		pShowPrice(a1,"Selling Price: %s WCoin",Price); 
	}
	else if(gOffTrade.TypeShop == 2)
	{
		pShowPrice(a1,"Selling Price: %s Jewel Of Soul",Price);  
	}
	else if(gOffTrade.TypeShop == 3)
	{
		pShowPrice(a1,"Selling Price: %s Jewel Of Bless",Price);  
	}
	else if(gOffTrade.TypeShop == 4)
	{
		pShowPrice(a1,"Selling Price: %s Jewel Of Chaos",Price);  
	}
	else if(gOffTrade.TypeShop == 5)
	{
		pShowPrice(a1,"Selling Price: %s Jewel Of Life",Price);  
	}
	else if(gOffTrade.TypeShop == 6)
	{
		pShowPrice(a1,"Selling Price: %s Jewel Of Creation",Price);  
	}
	else
	{
		pShowPrice(a1,"Selling Price: %s Zen",Price);  
	}
}

void cOffTrade::ShowTwoString(DWORD a1, const char *a2)
{
	if(gOffTrade.TypeShop == 1)
	{
		pShowPrice(a1,"You are short of WCoin"); 
	}
	else if(gOffTrade.TypeShop == 2)
	{
		pShowPrice(a1,"You are short of Soul");  
	}
	else if(gOffTrade.TypeShop == 3)
	{
		pShowPrice(a1,"You are short of Bless");  
	}
	else if(gOffTrade.TypeShop == 4)
	{
		pShowPrice(a1,"You are short of Chaos");  
	}
	else if(gOffTrade.TypeShop == 5)
	{
		pShowPrice(a1,"You are short of Life");  
	}
	else if(gOffTrade.TypeShop == 6)
	{
		pShowPrice(a1,"You are short of Creation");  
	}
	else
	{
		pShowPrice(a1,"You are short of Zen");  
	}
}