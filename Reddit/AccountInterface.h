#pragma once
#include "accountEnums.h"
#include "commentUWPitem.h"
#include <string>
#include <queue>
#include <ppltasks.h>
#include "subpost.h"
#include "subpostUWP.h"
#include <functional>
#define JsonGetString(vname, jname, jobj) try{vname = jobj->GetNamedString(jname);} catch(Platform::COMException^ e){globalvars::LogChannel->LogMessage(e->Message + L" String:" + jname);}
#define JsonGetStringWithDefault(vname, jname, jobj,defaultvalue) try{vname = jobj->GetNamedString(jname, defaultvalue);} catch(Platform::COMException^ e){globalvars::LogChannel->LogMessage(e->Message + L" String:" + jname);}
#define JsonGetBool(vname, jname, jobj) try{vname = jobj->GetNamedBoolean(jname);} catch(Platform::COMException^ e){globalvars::LogChannel->LogMessage(e->Message + L" Bool:" + jname);}
#define JsonGetBooleanWithDefault(vname, jname, jobj,defaultvalue) try{vname = jobj->GetNamedBoolean(jname, defaultvalue);} catch(Platform::COMException^ e){globalvars::LogChannel->LogMessage(e->Message + L" Bool" + jname);}
#define JsonGetNumberWithDefault(vname, jname, jobj,defaultvalue) try{vname = jobj->GetNamedNumber(jname, defaultvalue);} catch(Platform::COMException^ e){globalvars::LogChannel->LogMessage(e->Message + L" Number" + jname);}
#define JsonGetNumber(vname, jname, jobj) try{vname = jobj->GetNamedNumber(jname);} catch(Platform::COMException^ e){globalvars::LogChannel->LogMessage(e->Message + L" Number:" + jname);}

namespace account
{
	struct NewAccountLoginFailure : std::exception
	{
		enum class reason : char{
			UserCanceled, networkerror
		}Reason;
		std::wstring message;
		NewAccountLoginFailure(reason r) : Reason(r) {}
		NewAccountLoginFailure(reason r, std::wstring str) : Reason(r), message(str) {}
	};
	public value struct AccountInfo sealed
	{
		Platform::String^ username;
		int link_karma;
		int comment_karma;
		bool has_gold;
		bool has_mail;
		int64 bitflag;
	};
	class AccountInterface sealed
	{
	private:
		ref class authFilter sealed : public Windows::Web::Http::Filters::IHttpFilter
		{
		public:
			virtual ~authFilter() {}
			authFilter(Windows::Web::Http::Filters::IHttpFilter^ innerFilter)
			{
				this->innerFilter = innerFilter;
			}
			authFilter(Windows::Web::Http::Filters::IHttpFilter^ innerFilter, Platform::String^ refreshtoken);
			authFilter(Windows::Web::Http::Filters::IHttpFilter^ innerFilter, Platform::String^ refreshtoken, Platform::String^ currentToken);
			virtual Windows::Foundation::IAsyncOperationWithProgress<
				Windows::Web::Http::HttpResponseMessage^,
				Windows::Web::Http::HttpProgress>^ SendRequestAsync(Windows::Web::Http::HttpRequestMessage^ request);

		private:
			Windows::Web::Http::Headers::HttpCredentialsHeaderValue^ AuthHeader = nullptr;
			Platform::String^ refreshToken;
			concurrency::task<void> refreshauth();
			Windows::Web::Http::Filters::IHttpFilter^ innerFilter;
		};

		
		static AccountInfo jsontoinfo(Windows::Data::Json::JsonObject^ jsoninfo);
		static const std::wstring apibase;
		static Platform::String^ baseURI;
		concurrency::task<Windows::Data::Json::JsonObject^> getJsonAsync(Windows::Foundation::Uri^ requestUri, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
	public:
		enum class Messages_Where : byte
		{
			inbox, unread, sent
		};
		concurrency::task<std::pair<Platform::Collections::Vector<IRedditTypeIdentifier^>^,const Platform::String^>> getMessages(Messages_Where m = Messages_Where::inbox, concurrency::cancellation_token cToken =  concurrency::cancellation_token::none());
		concurrency::task<void> updateInfo(concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		Concurrency::task<void> changeSave(Platform::String^ fullname, bool dir, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<Windows::Data::Json::JsonObject^> getJsonFromBasePath(Platform::String^ path, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<std::vector<reportReason>> getRules(Platform::String^ subreddit, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		AccountInfo me;
		AccountInterface();
		AccountInterface(Platform::String^ refresh, AccountInfo cachedInfo);
		AccountInterface(Platform::String^ refresh);
		AccountInterface(Platform::String^ refresh, Platform::String^ currentauth);
		concurrency::task<subredditInfo> GetSubredditInfo(Platform::String^ subreddit, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<subredditInfo> GetSubredditInfo(subredditInfo s, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		Concurrency::task<void> vote(Platform::IBox<bool>^ dir, Platform::String^ id, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<void> giveGold(Platform::String^ fullname, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<Windows::Web::Http::HttpResponseMessage^> comment(Platform::String^ ID, Platform::String^ md, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		static Concurrency::task<AccountInterface*> LoginNewAcc(concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<Windows::Data::Json::JsonObject^> getObjectInfo(Platform::String^ fullname, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<Windows::Data::Json::JsonObject^> getObjectInfo(std::queue<Platform::String^> fullnames, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<commentUWPlisting> getCommentAsyncVec(Platform::String^ ID, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<commentUWPlisting> getCommentAsyncVec(Platform::String^ ID, commentSort sort, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<commentUWPlisting> getmorecomments(moreComments^ more, Platform::String^ link_id, Platform::String^ parent_id, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		std::unique_ptr<subredditlisting> getsubredditAsyncVec(concurrency::cancellation_token cToken = concurrency::cancellation_token::none()); //assume get frontpage if no subreddit argument
		std::unique_ptr<subredditlisting> getsubredditAsyncVec(Platform::String^ subreddit, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		std::unique_ptr<subredditlisting> getsubredditAsyncVec(Platform::String^ subreddit, postSort sort, timerange range, concurrency::cancellation_token cToken = concurrency::cancellation_token::none()); //returns an empty vector that async populates
		Windows::Web::Http::HttpClient^ httpClient;
		concurrency::task<Platform::Collections::Vector<IRedditTypeIdentifier^>^> getDomain(Platform::String^ domain, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		concurrency::task<Platform::Collections::Vector<IRedditTypeIdentifier^>^> getMixedCollection(Platform::String^ endpoint, concurrency::cancellation_token cToken = concurrency::cancellation_token::none());
		static IRedditTypeIdentifier^ ambiguousJsonResolver(Windows::Data::Json::JsonObject^ j);
	};

}
namespace std
{
	template<>
	struct std::equal_to<account::AccountInfo> sealed
	{
		bool operator () (const account::AccountInfo &  a, const account::AccountInfo & b)
		{
			return a.username == b.username;
		}
	};
}