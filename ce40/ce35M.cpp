#include "stdafx.h"
#include "CompactDog.h"
//#include "DataProvider.h"


__declspec(dllexport) DataProvider* GetPlugin()
{
	return new CompactDog();
}