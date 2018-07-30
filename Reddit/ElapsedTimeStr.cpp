#include "pch.h"
#include "ElapsedTimeStr.h"

template <size_t n>
std::wstring getPluralized(long cnt,const wchar_t (&unit)[n])
{
	std::wstring str = std::to_wstring(cnt) + unit;
	if (cnt > 1)
	{
		str += L's';
	}
	str += L" ago";
	return str;
}

std::wstring Reddit::ElapsedTimeStr::getStringFromTimeDiff(unsigned long diff)
{
	if (diff < 60)
	{
		return L"1 minute ago"; //round less than one minute to one minute
	}
	else if (diff < 3600) //  in minutes
	{
		return getPluralized(diff / 60, L" minute");
	}
	else if (diff < 86400) // in hours
	{
		return getPluralized(diff / 3600,  L" hour");
	}
	else if (diff < ((unsigned long)2.628e6)) // in days
	{
		return getPluralized(diff / 86400, L" day");
	}
	else if (diff < ((unsigned long)3.0222e7)) //in months
	{
		return getPluralized(diff / (unsigned long)2.628e6,  L" month");
	}
	else //in years
	{
		return getPluralized(diff / (unsigned long)3.0222e7 , L" year");
	}
}

std::wstring Reddit::ElapsedTimeStr::getStringFromCreatedUTC(unsigned long created)
{
	auto x = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	return getStringFromTimeDiff(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - created);
}
