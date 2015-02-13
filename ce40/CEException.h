

#ifndef CEEXCEPTION_H
#define CEEXCEPTION_H

#include "stdafx.h"
using namespace std;

class CEException : public std::exception
{
protected:
	string _m_what;
	wstring _wm_what;
public:
	CEException() throw(){};
	CEException(const char* message) throw()
	{
		_m_what = message;
	}
	CEException(wstring message, bool errorinfo = false)
	{
		if( !errorinfo )
		{
			_wm_what = message;
		}
		else
		{
			BSTR errtext;
			IErrorInfo* err = NULL;
			
			GetErrorInfo(0, &err);
			err->GetDescription(&errtext);
			
			_wm_what += L"Hatâ: \n\n";
			_wm_what += L"Esnâ: ";
			_wm_what += message;
			_wm_what += L"\n";
			
			SysFreeString(errtext);
		}
	}
	const char* what() throw()
	{
	    return _m_what.c_str();
	}
	const wchar_t* wwhat() throw()
	{
		return _wm_what.c_str();
	}
	~CEException()throw(){}
};

#endif//CEEXCEPTION_H