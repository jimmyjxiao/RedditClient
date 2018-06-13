#include "pch.h"
#include <cctype>
namespace globalvars
{
	settings appSettingCache;
	Windows::Foundation::Diagnostics::LoggingChannel^ LogChannel;
	Windows::Web::Http::HttpClient^ generalHttp;
	std::map<std::wstring, account::AccountInterface*, ci_less> accounts;
    account::AccountInterface* currentacc;
	std::vector<navVariant> NavState;
	unsigned char navChar = -1;
	Windows::Web::Http::HttpClient^ imgurHttp;
	int htmlinstance = 0;
	std::vector<std::pair<Platform::String^, Platform::Collections::Vector<account::reportReason>^>> reportReasonCache;
	void clearOldPageCaches()
	{
		if (NavState.size() > 3)
		{
			(NavState.end() - 3)->second->pageState = nullptr;
		}
	}
	unsigned char addNav(Windows::UI::Xaml::Interop::TypeName pageType, baseNavState * c)
	{
		navChar++;
		NavState.emplace_back(pageType,c);
		clearOldPageCaches();
		return navChar;
	}
	
	concurrency::task<void> UpdateAccountsTask()
	{
		std::vector<concurrency::task<void>> updateTasks = std::vector<concurrency::task<void>>(globalvars::accounts.size());
		std::transform(globalvars::accounts.begin(), globalvars::accounts.end(), updateTasks.begin(), [](std::pair<std::wstring, account::AccountInterface*> a) {
			return a.second->updateInfo();
		});
		return concurrency::when_all(updateTasks.begin(), updateTasks.end());
	}
	bool ci_less::nocase_compare::operator()(const wchar_t & c1, const wchar_t & c2) const
	{
			return std::tolower(c1) < std::tolower(c2);
	}
	bool ci_less::operator()(const std::wstring & s1, const std::wstring & s2) const
	{
		return std::lexicographical_compare
		(s1.begin(), s1.end(),   // source range
			s2.begin(), s2.end(),   // dest range
			nocase_compare());  // comparison
	}
}