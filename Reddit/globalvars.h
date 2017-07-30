#pragma once
#include "AccountInterface.h"
namespace globalvars
{
	extern Windows::Web::Http::HttpClient^ generalHttp; //httpclient to use for general purpose, non-account specific stuff
	extern Windows::Web::Http::HttpClient^ imgurHttp;
	extern std::map<std::wstring, account::AccountInterface*> accounts;
	extern std::map<unsigned char, void*> NavState;
	extern unsigned char navChar;
	extern account::AccountInterface * currentacc;
}