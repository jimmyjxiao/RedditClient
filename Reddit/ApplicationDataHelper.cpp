#include "pch.h"
#include "ApplicationDataHelper.h"
#include "sqlite_modern_cpp\hdr\sqlite_modern_cpp.h"
#include <optional>
namespace globalvars
{
	sqlite::database * AppDB;
}
namespace ApplicationDataHelper
{
	namespace rulesOneUnitCache
	{
		std::vector<account::reportReason> oneUnitCache;
		std::u16string subredditCachedName;
		unsigned int index = INT_MAX;
		std::mutex mutex;
	}
	void initializeAppDB()
	{
		auto path = Windows::Storage::ApplicationData::Current->LocalFolder->Path + L"\\AppDB.db";
		globalvars::AppDB = new sqlite::database((const char16_t*)path->Data());
		try
		{
			//create subredditinfo table
			*globalvars::AppDB << u"CREATE TABLE IF NOT EXISTS 'SubredditInfo' ( `subreddit` TEXT UNIQUE COLLATE NOCASE PRIMARY KEY, `key_color` INTEGER, `sidebar_html` TEXT, `NSFW` INTEGER, `submissionType` INTEGER, `subscribers` INTEGER, `desc` TEXT, `lastModified` INTEGER , 'subscribedUsers' INTEGER )";
		}
		catch (sqlite::sqlite_exception e)
		{
			__debugbreak();
		}
		//create subredditRules cache table
		*globalvars::AppDB <<
		u"CREATE TABLE IF NOT EXISTS 'SubredditRulesCache' ( `subredditIndex` INTEGER, `ruleNumber` INTEGER, `shortDesc` TEXT, `longDesc` TEXT, `applic` INTEGER )";

		//create myusers table
		try
		{
			*globalvars::AppDB
				<<
				u"CREATE TABLE IF NOT EXISTS `MyUsers` ( `username` TEXT NOT NULL PRIMARY KEY, `link_karma` INTEGER NOT NULL, `comment_karma` INTEGER NOT NULL, `has_gold` INTEGER NOT NULL )";
		}
		catch (sqlite::sqlite_exception e)
		{
			__debugbreak();
		}
		
	}
	sqlite::database_binder& RulesPrepSt()
	{
		static auto prepst = *globalvars::AppDB << u"SELECT shortDesc, longDesc, applic FROM SubredditRulesCache WHERE subredditIndex = ? ORDER BY ruleNumber ASC";
		//prepst.used(true);
		return prepst;
	}
	int FindSubIndex(std::u16string &sub)
	{
		static auto prepst = *globalvars::AppDB << u"SELECT rowid FROM SubredditInfo WHERE subreddit = ?";
		try {
			int i;
			prepst << sub >> i;
			return i;
		}
		catch (sqlite::sqlite_exception e)
		{
			static auto insert = *globalvars::AppDB << u"INSERT INTO SubredditInfo (subreddit, NSFW) VALUES (?, 999)";
			insert << sub;
			insert.execute();
			return globalvars::AppDB->last_insert_rowid();
		}
	}
	Platform::Collections::Vector<account::reportReason>^ subredditHelpers::trysubredditRulesCache(std::u16string subredditName, Platform::Collections::Vector<account::reportReason>^ v, concurrency::cancellation_token cToken)
	{
		std::lock_guard guard(rulesOneUnitCache::mutex);
		if (subredditName == rulesOneUnitCache::subredditCachedName)
		{
			v->ReplaceAll(Platform::ArrayReference<account::reportReason>(rulesOneUnitCache::oneUnitCache.data(), rulesOneUnitCache::oneUnitCache.size()));
			return std::move(v);
		}
		int index =
			FindSubIndex(subredditName);
		
		std::vector<account::reportReason> newvec;
		RulesPrepSt() << index
			>>
			[&newvec](std::u16string shortDesc, std::u16string longdesc, int applic) {
			newvec.emplace_back( account::reportReason{ ref new Platform::String((const wchar_t*)shortDesc.data()), ref new Platform::String((const wchar_t*)longdesc.data()), (account::reportApplicibility)applic
			});
		};
		if (newvec.size() != 0)
		{
			rulesOneUnitCache::subredditCachedName = subredditName;
			rulesOneUnitCache::oneUnitCache = std::move(newvec);
			rulesOneUnitCache::index = index;
			v->ReplaceAll(Platform::ArrayReference<account::reportReason>(rulesOneUnitCache::oneUnitCache.data(), rulesOneUnitCache::oneUnitCache.size()));
			return std::move(v);
		}
		else
		{
			cacheMiss<std::vector<account::reportReason>> missException;
			missException.retrieveTask = globalvars::currentacc->getRules(Platform::StringReference((const wchar_t*)subredditName.data()), cToken).then([v, index, subredditName](std::vector<account::reportReason> e) {
				v->ReplaceAll(Platform::ArrayReference<account::reportReason>(e.data(), e.size()));
				std::lock_guard guard(rulesOneUnitCache::mutex);
				storeSubredditRules(e, index);
				rulesOneUnitCache::subredditCachedName = std::move(subredditName);
				rulesOneUnitCache::index = index;
				rulesOneUnitCache::oneUnitCache = std::move(e);
				return rulesOneUnitCache::oneUnitCache;
			});
			throw missException;
		}
	}
	Platform::Collections::Vector<account::reportReason>^ subredditHelpers::trysubredditRulesCache(unsigned int index, Platform::Collections::Vector<account::reportReason>^ v , concurrency::cancellation_token cToken)
	{
		std::lock_guard guard(rulesOneUnitCache::mutex);
		if (index == rulesOneUnitCache::index)
		{
			v->ReplaceAll(Platform::ArrayReference<account::reportReason>(rulesOneUnitCache::oneUnitCache.data(), rulesOneUnitCache::oneUnitCache.size()));
			return std::move(v);
		}
		std::vector<account::reportReason> newvec;
		RulesPrepSt() << index
			>>
			[&newvec](std::u16string shortDesc, std::u16string longdesc, int applic) {
			newvec.emplace_back(account::reportReason{ ref new Platform::String((const wchar_t*)shortDesc.data()), ref new Platform::String((const wchar_t*)longdesc.data()), (account::reportApplicibility)applic
				});
		};
		static auto getname = *globalvars::AppDB << u"SELECT subreddit FROM SubredditInfo WHERE rowid = ?";
		std::u16string subredditName;
		try
		{
			getname << index >> subredditName;
		}
		catch (std::exception e)
		{
			__debugbreak();
		}
		if (v->Size > 0)
		{
			rulesOneUnitCache::oneUnitCache = std::move(newvec);
			rulesOneUnitCache::index = index;
			rulesOneUnitCache::subredditCachedName = std::move(subredditName);
			return v;
		}
		else
		{
			cacheMiss<std::vector<account::reportReason>> missException;
			missException.retrieveTask = globalvars::currentacc->getRules(Platform::StringReference((const wchar_t*)subredditName.data()), std::move(cToken)).then([v, index, subredditName](std::vector<account::reportReason> e) {
				v->ReplaceAll(Platform::ArrayReference<account::reportReason>(e.data(), e.size()));
				std::lock_guard guard(rulesOneUnitCache::mutex);
				storeSubredditRules(e, index);
				rulesOneUnitCache::subredditCachedName = std::move(subredditName);
				rulesOneUnitCache::index = index;
				rulesOneUnitCache::oneUnitCache = std::move(e);
				return rulesOneUnitCache::oneUnitCache;
			});
			throw missException;
		}
	}
	account::subredditInfo subredditHelpers::trysubredditInfoCache(std::u16string subredditName, concurrency::cancellation_token cToken)
	{
		static account::subredditInfo oneUnitCache; //it happens pretty often that we are in a subreddit, and the user clicks on a comment thread inside that subreddit. Rather than using sqlite and retrieving subredditinfo again, we'll just reuse it
		if (std::wcscmp(oneUnitCache.name->Data(), (const wchar_t*)subredditName.data()) == 0)
		{
			return oneUnitCache;
		}
		std::u16string name;
		std::optional<int32> intcolor;
		int NSFW;
		std::u16string sidebar_html;
		int subs;
		char subType;
		int index = INT_MIN;
		int64 bitflags;
		try
		{
			static auto prepSt = *globalvars::AppDB <<
				u"SELECT subreddit, key_color, sidebar_html, NSFW, submissionType, subscribers, rowid, subscribedUsers "
				"FROM SubredditInfo "
				"WHERE subreddit = ?";
			prepSt
				<< subredditName
				>>
				std::tie(name, intcolor, sidebar_html, NSFW, subType, subs, index, bitflags);
			if (NSFW == 999)
			{
				throw std::exception("Found record, but record is unintialized");
			}
			account::subredditInfo z =
			{
				ref new Platform::String((const wchar_t*)name.data()),
				ref new Platform::String((const wchar_t*)(u"/r/" + name).data()),
				ref new Platform::String((const wchar_t*)sidebar_html.data()),
				nullptr,
				inttocolor(intcolor),
				(bool)NSFW,
				subs,
				(account::subType)subType,
				bitflags,
				index
			};
			oneUnitCache = z;
			return z;
		}
		catch (std::exception e)
		{
			cacheMiss<account::subredditInfo> except(std::move(e));
			except.retrieveTask = globalvars::currentacc->GetSubredditInfo(Platform::StringReference((const wchar_t*)subredditName.data()), std::move(cToken)).then([index](account::subredditInfo x) {
				x.subredditIndex = index;
				storeSubredditInfo(x);
				return x;
			});
			throw except;
		}
	}
	Platform::Collections::Vector<account::subredditInfo, account::subInfoCompare>^ subredditHelpers::SearchSubNames(const char16_t * str, bool bothways)
	{
		auto returning = ref new Platform::Collections::Vector<account::subredditInfo, account::subInfoCompare>();
		try {
			std::u16string query = u"SELECT subreddit, key_color, sidebar_html, NSFW, submissionType, subscribers, rowid, subscribedUsers  FROM SubredditInfo WHERE subreddit LIKE '";
			if (bothways)
				query += u'%';
			query.append(str);
			query += u"%'";
			*globalvars::AppDB << query
				>> [&returning](std::u16string sub, std::optional<int32> key_color,std::u16string sidebar, int NSFW, char subType, unsigned int subs, unsigned int index, int64 bitflag) {
				returning->Append(
				{
					Platform::StringReference((const wchar_t*)sub.data()),
					Platform::StringReference((const wchar_t*)(u"/r/" + sub).data()),
					Platform::StringReference((const wchar_t*)sidebar.data()),
					nullptr,
					inttocolor(key_color),
					(bool)NSFW,
					subs,
					(account::subType)subType,
					bitflag,
					index
				});
			};
			return returning;
		}
		catch (sqlite::sqlite_exception e)
		{
			__debugbreak();
		}
	}
	void subredditHelpers::storeSubredditInfo(account::subredditInfo &info)
	{
			try
			{
				if (info.subredditIndex == INT_MIN)
				{
					static auto prepst = *globalvars::AppDB <<
						u"INSERT INTO SubredditInfo "
						"(subreddit, key_color, sidebar_html, NSFW, submissionType, subscribers, desc, lastModified, subscribedUsers) "
						"VALUES (?,?,?,?,?,?,?, (SELECT datetime('now')), ?)";
					prepst
						<< (const char16_t*)(info.name->Data());
					if (info.key_color.A == Windows::UI::Colors::Transparent.A)
					{
						prepst
							<< nullptr;	
					}
					else
					{
						prepst
							<< colortoint(info.key_color);
					}
					prepst << (const char16_t*)(info.sidebar_html->Data())
						<< (int)info.NSFW
						<< (int)info.submissions
						<< info.subscribers
						<< (const char16_t*)info.desc->Data()
						<< info.subscribed;
					prepst.execute();
					info.subredditIndex = globalvars::AppDB->last_insert_rowid();
				}
				else
				{
					static auto prepst = *globalvars::AppDB <<
						u"UPDATE SubredditInfo "
						"SET key_color = ?, sidebar_html = ?, NSFW = ?, submissionType = ?, subscribers = ?, desc = ?, lastModified = (SELECT datetime('now')), (subscribedUsers | ?) "
						"WHERE rowid = ?";
					if (info.key_color.A == Windows::UI::Colors::Transparent.A)
					{
						prepst
							<< nullptr;
					}
					else
					{
						prepst
							<< colortoint(info.key_color);
					}
					prepst << (const char16_t*)(info.sidebar_html->Data())
						<< (int)info.NSFW
						<< (int)info.submissions
						<< info.subscribers
						<< (const char16_t*)info.desc->Data()
						<< info.subscribed
						<< info.subredditIndex;
					prepst.execute();
				}
			}
			catch (sqlite::sqlite_exception e)
			{
				__debugbreak();
			}
	}
	void subredditHelpers::storeSubredditRules(std::vector<account::reportReason>& r, std::u16string subname)
	{
		int i;
		try {
			i = FindSubIndex(subname);
		}
		catch (sqlite::sqlite_exception e)
		{
			*globalvars::AppDB << u"INSERT INTO SubredditInfo (subreddit) VALUES (?)" << subname;
			i = globalvars::AppDB->last_insert_rowid();
		}
		static auto StoreRow = *globalvars::AppDB << u"INSERT INTO SubredditRulesCache (subredditIndex, ruleNumber, shortDesc, longDesc, applic) VALUES (?,?,?,?,?)";
		for (std::vector<account::reportReason>::size_type n = 0; n != r.size(); n++)
		{
			StoreRow << i << (n + 1) << reinterpret_cast<const char16_t*>(r[n].shortdesc->Data()) << reinterpret_cast<const char16_t*>(r[n].violationReason) << static_cast<int>(r[n].scope);
			StoreRow.execute();
		}
	}
	void subredditHelpers::storeSubredditRules(std::vector<account::reportReason>& r, unsigned int index)
	{
		static auto StoreRow = *globalvars::AppDB << u"INSERT INTO SubredditRulesCache (subredditIndex, ruleNumber, shortDesc, longDesc, applic) VALUES (?,?,?,?,?)";
		for (std::vector<account::reportReason>::size_type n = 0; n != r.size(); n++)
		{
			StoreRow << index << (n + 1) << reinterpret_cast<const char16_t*>(r[n].shortdesc->Data()) << reinterpret_cast<const char16_t*>(r[n].violationReason) << static_cast<int>(r[n].scope);
			StoreRow.execute();
		}
	}
	int32 colortoint(Windows::UI::Color z)
	{
		int32 color = 0;
		//color |= z.A << 24;
		color |= z.R << 16;
		color |= z.G << 8;
		color |= z.B;
		////__debugbreak();
		return color;
	}
	Windows::UI::Color inttocolor(std::optional<int32> &number)
	{
		if (number.has_value())
		{
			return Windows::UI::ColorHelper::FromArgb(255, (number.value() >> 16 & 255), (number.value() >> 8 & 255), (number.value() & 255));
		}
		else
			return Windows::UI::Colors::Transparent;
		
	}
	void userHelpers::incrementUserVote(bool direction, std::u16string username)
	{
	}
	userHelpers::userInteractions userHelpers::getMyInteractionsWithUser(std::u16string username)
	{
		return userInteractions();
	}
	void userHelpers::cacheMyUser(account::AccountInfo & user)
	{
		static auto StoreRow = *globalvars::AppDB << u"INSERT INTO MyUsers (username, link_karma, comment_karma, has_gold) VALUES (?,?,?,?)";
		try {
			StoreRow << (const char16_t*)user.username->Data() << user.link_karma << user.comment_karma << (int)user.has_gold;
			StoreRow.execute();
			user.bitflag = (1 << globalvars::AppDB->last_insert_rowid());
		}
		catch (sqlite::sqlite_exception)
		{
			static auto updateRow = *globalvars::AppDB << u"UPDATE MyUsers SET link_karma = ?, comment_karma = ? , has_gold = ? WHERE username = ?";
			updateRow << user.link_karma << user.comment_karma << user.has_gold << (const char16_t*)user.username->Data();
			updateRow.execute();
		}
	}
	account::AccountInfo userHelpers::MyUserInfoCache(Platform::String^ username)
	{
		static auto GetRow = *globalvars::AppDB << u"SELECT link_karma, comment_karma, has_gold, (1 << rowid) FROM MyUsers WHERE username = ?";
		account::AccountInfo returning;
		int g;
		returning.username = std::move(username);
		GetRow << (const char16_t*)username->Data() >> std::tie(returning.link_karma, returning.comment_karma, g, returning.bitflag);
		returning.has_gold = std::move(g);
		return returning;
	}
}
