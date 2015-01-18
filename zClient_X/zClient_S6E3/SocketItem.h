#include "stdafx.h"

#ifdef _SOCKET_CONFIG_

#define MAX_ITEM 7680

class cSocketItem
{
public:
	void Load();
	static int CheckItem(signed int a1);


	struct
	{
		short Type;
		short Index;
	}sItem[MAX_ITEM];
	int ItemCount;


};
extern cSocketItem gSocketItem;
#endif