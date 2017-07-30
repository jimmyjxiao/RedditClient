#include "pch.h"
#include "globalvars.h"
namespace globalvars
{
	Windows::Web::Http::HttpClient^ generalHttp;
	std::map<std::wstring, account::AccountInterface*> accounts;
    account::AccountInterface* currentacc;
	std::map<unsigned char, void*> NavState;
	unsigned char navChar = 0;
	Windows::Web::Http::HttpClient^ imgurHttp;
}