#pragma once
#include <optional>
#include "subpostUWP.h"
namespace ApplicationDataHelper
{
	
	void initializeAppDB();
	int32 colortoint(Windows::UI::Color z);
	template<typename T>
	class cacheMiss : std::exception
	{
	public:
		cacheMiss() : std::exception("cache miss, querying reddit servers. Refer to task") {};
		cacheMiss(std::exception base) : std::exception(base)
		{
		}
		concurrency::task<T> retrieveTask;
	};
	Windows::UI::Color inttocolor(std::optional<int32> & number);

	namespace userHelpers
	{
		struct userInteractions
		{
			int myVotes;
			std::u16string tag;
		};
		void incrementUserVote(bool direction, std::u16string username);
		userInteractions getMyInteractionsWithUser(std::u16string username);
		void cacheMyUser(const account::AccountInfo & user);
		account::AccountInfo MyUserInfoCache(Platform::String^ username);
	}
	namespace subredditHelpers
	{
		Platform::Collections::Vector<account::reportReason>^ trysubredditRulesCache(std::u16string subredditName, Platform::Collections::Vector<account::reportReason>^ v = ref new Platform::Collections::Vector<account::reportReason>());
		Platform::Collections::Vector<account::reportReason>^ trysubredditRulesCache(unsigned int index, Platform::Collections::Vector<account::reportReason>^ v = ref new Platform::Collections::Vector<account::reportReason>());
		
		account::subredditInfo trysubredditInfoCache(std::u16string subredditName);
		Platform::Collections::Vector<account::subredditInfo, account::subInfoCompare>^ SearchSubNames(const char16_t* str, bool bothways);
	    void storeSubredditInfo(account::subredditInfo &info);
		void storeSubredditRules(std::vector<account::reportReason> &r, std::u16string subname);
		void storeSubredditRules(std::vector<account::reportReason> &r, unsigned int index);
	}
};
