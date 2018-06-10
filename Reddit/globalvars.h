#pragma once
#include "AccountInterface.h"
#include "sqlite_modern_cpp\hdr\sqlite_modern_cpp.h"
#include "subpostUWP.h"
#include <map>
namespace globalvars
{
	extern Windows::Foundation::Diagnostics::LoggingChannel^ LogChannel;
	struct ci_less
	{
		// case-independent (ci) compare_less binary function
		struct nocase_compare
		{
			bool operator() (const wchar_t& c1, const  wchar_t& c2) const {
				return std::tolower(c1) < std::tolower(c2);
			}
		};
		bool operator() (const std::wstring & s1, const std::wstring & s2) const {
			return std::lexicographical_compare
			(s1.begin(), s1.end(),   // source range
				s2.begin(), s2.end(),   // dest range
				nocase_compare());  // comparison
		}
	};
	struct settings
	{
		enum class ReplyMode : bool
		{
			enhanced = false, md = true
		} initialReplyMode : 1;
		enum class defaultReplyMode : unsigned char
		{
			enhanced = 0, md = 1, lastUsed = 2
		} replyModeSetting : 2;
	} extern appSettingCache;
	const int hiddenScore = INT_MIN + 1;
	extern std::vector<std::pair<Platform::String^, Platform::Collections::Vector<account::reportReason>^>> reportReasonCache;
	extern Windows::Web::Http::HttpClient^ generalHttp; //httpclient to use for general purpose, non-account specific stuff
	extern Windows::Web::Http::HttpClient^ imgurHttp;
	extern int htmlinstance;
	extern std::map<std::wstring, account::AccountInterface*, ci_less> accounts;
	extern std::vector<navVariant> NavState;
	unsigned char addNav(Windows::UI::Xaml::Interop::TypeName pageType, baseNavState * c);
	extern unsigned char navChar;
	extern account::AccountInterface * currentacc;
	extern sqlite::database * AppDB;
	concurrency::task<void> UpdateAccountsTask();
}