#pragma once
#include "accountEnums.h"
#include "commentUWPitem.h"
#include <string>
#include <queue>
#include <ppltasks.h>
#include "subpost.h"
#include "subpostUWP.h"
#include <functional>
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
		concurrency::task<Windows::Data::Json::JsonObject^> getJsonAsync(Windows::Foundation::Uri^ requestUri);
	public:
		concurrency::task<void> updateInfo();
		Concurrency::task<void> changeSave(Platform::String^ fullname, bool dir);
		concurrency::task<Windows::Data::Json::JsonObject^> getJsonFromBasePath(Platform::String^ path);
		concurrency::task<std::vector<reportReason>> getRules(Platform::String^ subreddit);
		AccountInfo me;
		AccountInterface();
		AccountInterface(Platform::String^ refresh, AccountInfo cachedInfo);
		AccountInterface(Platform::String^ refresh);
		AccountInterface(Platform::String^ refresh, Platform::String^ currentauth);
		Concurrency::task<void> vote(Platform::IBox<bool>^ dir, Platform::String^ id);
		concurrency::task<Windows::Web::Http::HttpResponseMessage^> comment(Platform::String^ ID, Platform::String^ md);
		static Concurrency::task<AccountInterface*> LoginNewAcc();
		concurrency::task<Windows::Data::Json::JsonObject^> getObjectInfo(Platform::String^ fullname);
		concurrency::task<Windows::Data::Json::JsonObject^> getObjectInfo(std::queue<Platform::String^> fullnames);
		concurrency::task<commentUWPlisting> getCommentAsyncVec(Platform::String^ ID);
		concurrency::task<commentUWPlisting> getCommentAsyncVec(Platform::String^ ID, commentSort sort);
		concurrency::task<commentUWPlisting> getmorecomments(moreComments^ more, Platform::String^ link_id, Platform::String^ parent_id);
		std::unique_ptr<subredditlisting> getsubredditAsyncVec(); //assume get frontpage if no subreddit argument
		std::unique_ptr<subredditlisting> getsubredditAsyncVec(Platform::String^ subreddit);
		std::unique_ptr<subredditlisting> getsubredditAsyncVec(Platform::String^ subreddit, postSort sort, timerange range); //returns an empty vector that async populates
		Windows::Web::Http::HttpClient^ httpClient;
	
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