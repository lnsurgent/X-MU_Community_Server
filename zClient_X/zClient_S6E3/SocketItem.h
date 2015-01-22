#include "stdafx.h"

#define MAX_ITEM 7680
#ifdef __LOL__
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