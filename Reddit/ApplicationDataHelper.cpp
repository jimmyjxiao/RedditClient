#include "pch.h"
#include "ApplicationDataHelper.h"
#include "sqlite_modern_cpp\hdr\sqlite_modern_cpp.h"
#include <optional>
namespace ApplicationDataHelper
{
	namespace rulesOneUnitCache
	{
		Platform::Collections::Vector<account::reportReason>^ oneUnitCache;
		std::u16string subredditCachedName;
		unsigned int index = INT_MAX;
	}
	void initializeAppDB()
	{
		auto path = Windows::Storage::ApplicationData::Current->LocalFolder->Path + L"\\AppDB.db";
		globalvars::AppDB = new sqlite::database((const char16_t*)path->Data());

		//create subredditinfo table
		*globalvars::AppDB << u"CREATE TABLE IF NOT EXISTS 'SubredditInfo' ( `index` INTEGER PRIMARY KEY AUTOINCREMENT, `subreddit` TEXT UNIQUE COLLATE NOCASE, `key_color` INTEGER, `sidebar_html` TEXT, `NSFW` INTEGER, `submissionType` INTEGER, `subscribers` INTEGER, `desc` TEXT, `lastModified` INTEGER )";
		
		//create subredditRules cache table
		*globalvars::AppDB <<
		u"CREATE TABLE IF NOT EXISTS 'SubredditRulesCache' ( `subredditIndex` INTEGER, `ruleNumber` INTEGER, `shortDesc` TEXT, `longDesc` TEXT, `applic` INTEGER )";

		//create myusers table
		try
		{
			*globalvars::AppDB
				<<
				u"CREATE TABLE IF NOT EXISTS `MyUsers` ( `username` TEXT NOT NULL UNIQUE, `link_karma` INTEGER NOT NULL, `comment_karma` INTEGER NOT NULL, `has_gold` INTEGER NOT NULL )";
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
		static auto prepst = *globalvars::AppDB << u"SELECT [index] FROM SubredditInfo WHERE subreddit = ?";
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
	Platform::Collections::Vector<account::reportReason>^ subredditHelpers::trysubredditRulesCache(std::u16string subredditName, Platform::Collections::Vector<account::reportReason>^ v)
	{
		if (subredditName == rulesOneUnitCache::subredditCachedName)
		{
			return rulesOneUnitCache::oneUnitCache;
		}
		int index =
			FindSubIndex(subredditName);
		
		
		RulesPrepSt() << index
			>>
			[&v](std::u16string shortDesc, std::u16string longdesc, int applic) {
			v->Append(account::reportReason{ Platform::StringReference((const wchar_t*)shortDesc.data()), Platform::StringReference((const wchar_t*)longdesc.data()), (account::reportApplicibility)applic });
		};
		rulesOneUnitCache::subredditCachedName = subredditName;
		rulesOneUnitCache::oneUnitCache = v;
		rulesOneUnitCache::index = index;
		if (!v->Size == 0)
		{
			return v;
		}
		else
		{
			cacheMiss<std::vector<account::reportReason>> missException;
			missException.retrieveTask = globalvars::currentacc->getRules(Platform::StringReference((const wchar_t*)subredditName.data())).then([v, index](std::vector<account::reportReason> e) {
				storeSubredditRules(e, index);
				v->ReplaceAll(ref new Platform::Array<account::reportReason>(e.data(), e.size()));
				return e;
			});
			throw missException;
		}
	}
	Platform::Collections::Vector<account::reportReason>^ subredditHelpers::trysubredditRulesCache(unsigned int index, Platform::Collections::Vector<account::reportReason>^ v )
	{
		if (rulesOneUnitCache::index == index)
		{
			return rulesOneUnitCache::oneUnitCache;
		}
		RulesPrepSt() << index
			>>
			[&v](std::u16string shortDesc, std::u16string longdesc, int applic) {
			v->Append(account::reportReason{ Platform::StringReference((const wchar_t*)shortDesc.data()), Platform::StringReference((const wchar_t*)longdesc.data()), (account::reportApplicibility)applic });
		};
		static auto getname = *globalvars::AppDB << u"SELECT subreddit FROM SubredditInfo WHERE [index] = ?";
		std::u16string subredditName;
		try
		{
			getname << index >> subredditName;
		}
		catch (std::exception e)
		{
			__debugbreak();
		}
		
		rulesOneUnitCache::oneUnitCache = v;
		rulesOneUnitCache::index = index;
		if (!v->Size == 0)
		{
			rulesOneUnitCache::subredditCachedName = std::move(subredditName);
			return v;
		}
		else
		{
			rulesOneUnitCache::subredditCachedName = subredditName;
			cacheMiss<std::vector<account::reportReason>> missException;
			missException.retrieveTask = globalvars::currentacc->getRules(Platform::StringReference((const wchar_t*)subredditName.data())).then([v, index](std::vector<account::reportReason> e) {
				storeSubredditRules(e, index);
				v->ReplaceAll(ref new Platform::Array<account::reportReason>(e.data(), e.size()));
				return e;
			});
			throw missException;
		}
	}
	account::subredditInfo subredditHelpers::trysubredditInfoCache(std::u16string subredditName)
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
		try
		{
			static auto prepSt = *globalvars::AppDB <<
				u"SELECT subreddit, key_color, sidebar_html, NSFW, submissionType, subscribers, [index] "
				"FROM SubredditInfo "
				"WHERE subreddit = ?";
			prepSt
				<< subredditName
				>>
				std::tie(name, intcolor, sidebar_html, NSFW, subType, subs, index);
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
				false,
				index
			};
			oneUnitCache = z;
			return z;
		}
		catch (std::exception e)
		{
			cacheMiss<account::subredditInfo> except(e);
			except.retrieveTask =
			concurrency::create_task(globalvars::currentacc->getJsonFromBasePath(Platform::StringReference((const wchar_t*)(u"r/" + subredditName + u"/about.json?raw_json=1").data())).then([index](Windows::Data::Json::JsonObject^ response) {
				auto info = account::subpost::getSubredditInfoFromJson(response->GetNamedObject("data"));
				info.subredditIndex = index;
				storeSubredditInfo(info);
				return info;
			}));
			throw except;
		}
	}
	Platform::Collections::Vector<account::subredditInfo, account::subInfoCompare>^ subredditHelpers::SearchSubNames(const char16_t * str, bool bothways)
	{
		auto returning = ref new Platform::Collections::Vector<account::subredditInfo, account::subInfoCompare>();
		try {
			std::u16string query = u"SELECT subreddit, key_color, sidebar_html, NSFW, submissionType, subscribers, [index] FROM SubredditInfo WHERE subreddit LIKE '";
			if (bothways)
				query += u'%';
			query.append(str);
			query += u"%'";
			*globalvars::AppDB << query
				>> [&returning](std::u16string sub, std::optional<int32> key_color,std::u16string sidebar, int NSFW, char subType, unsigned int subs, unsigned int index) {
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
					false,
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
						"(subreddit, key_color, sidebar_html, NSFW, submissionType, subscribers, desc, lastModified) "
						"VALUES (?,?,?,?,?,?,?, (SELECT datetime('now')))";
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
						<< (const char16_t*)info.desc->Data();
					prepst.execute();
					info.subredditIndex = globalvars::AppDB->last_insert_rowid();
				}
				else
				{
					static auto prepst = *globalvars::AppDB <<
						u"UPDATE SubredditInfo "
						"SET key_color = ?, sidebar_html = ?, NSFW = ?, submissionType = ?, subscribers = ?, desc = ?, lastModified = (SELECT datetime('now')) "
						"WHERE [index] = ?";
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
	void userHelpers::cacheMyUser(const account::AccountInfo & user)
	{
		static auto StoreRow = *globalvars::AppDB << u"INSERT OR REPLACE INTO MyUsers (username, link_karma, comment_karma, has_gold) VALUES (?,?,?,?)";
		StoreRow << (const char16_t*)user.username->Data() << user.link_karma << user.comment_karma << (int)user.has_gold;
		StoreRow.execute();
	}
	account::AccountInfo userHelpers::MyUserInfoCache(Platform::String^ username)
	{
		static auto GetRow = *globalvars::AppDB << u"SELECT link_karma, comment_karma, has_gold FROM MyUsers WHERE username = ?";
		account::AccountInfo returning;
		int g;
		returning.username = std::move(username);
		GetRow << (const char16_t*)username->Data() >> std::tie(returning.link_karma, returning.comment_karma, g);
		returning.has_gold = std::move(g);
		return returning;
	}
}
