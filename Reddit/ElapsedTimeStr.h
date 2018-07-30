#pragma once
namespace Reddit
{
	namespace ElapsedTimeStr
	{
		std::wstring getStringFromTimeDiff(unsigned long diff);
		std::wstring getStringFromCreatedUTC(unsigned long created);
	};
}