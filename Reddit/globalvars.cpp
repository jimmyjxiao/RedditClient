#include "pch.h"
#include "globalvars.h"
namespace globalvars
{
	settings appSettingCache;
	
	Windows::Web::Http::HttpClient^ generalHttp;
	std::map<std::wstring, account::AccountInterface*, ci_less> accounts;
    account::AccountInterface* currentacc;
	std::vector<navVariant> NavState;
	unsigned char navChar = -1;
	Windows::Web::Http::HttpClient^ imgurHttp;
	sqlite::database * AppDB;
	int htmlinstance = 0;
	std::vector<std::pair<Platform::String^, Platform::Collections::Vector<account::reportReason>^>> reportReasonCache;
	void clearOldPageCaches()
	{
		if (NavState.size() > 3)
		{
			static_cast<baseNavState*>((NavState.end() - 3)->second.s)->pageState = nullptr;
		}
	}
	unsigned char addNav(commentNavstate * c)
	{
		navChar++;
		NavState.emplace_back(NavTypes::comment,c);
		clearOldPageCaches();
		return navChar;
	}
	unsigned char addNav(subredditNavstate * c)
	{
		navChar++;
		NavState.emplace_back( NavTypes::subreddit,c);
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
}