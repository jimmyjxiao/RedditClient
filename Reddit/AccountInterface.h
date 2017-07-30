#pragma once
#include "commentUWPitem.h"
#include <string>
#include <queue>
#include <ppltasks.h>
#include "subpost.h"
#include "subpostUWP.h"
namespace account
{
	
	public enum class timerange
	{
		hour, day, week, month, year, all, Default
	};
	public enum class postSort
	{
		hot, New, rising, controversial, top, gilded, Defaultsort
	};
	public enum class commentSort
	{
		best, top, New, controversial, old, qa, default, random
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

		concurrency::task<AccountInfo> getinfo();
		static AccountInfo jsontoinfo(Windows::Data::Json::JsonObject^ jsoninfo);
		static const std::wstring apibase;
		static Platform::String^ baseURI;
		concurrency::task<Windows::Data::Json::JsonObject^> getJsonAsync(Windows::Foundation::Uri^ requestUri);
	public:
		Concurrency::task<void> changeSave(Platform::String^ fullname, bool dir);
		concurrency::task<Windows::Data::Json::JsonObject^> getJsonFromBasePath(Platform::String^ path);
		AccountInfo me;
		AccountInterface();
		AccountInterface(Platform::String^ refresh);
		AccountInterface(Platform::String^ refresh, Platform::String^ currentauth);
		Concurrency::task<void> vote(Platform::IBox<bool>^ dir, Platform::String^ id);

		static Concurrency::task<AccountInterface*> LoginNewAcc();
		concurrency::task<Windows::Data::Json::JsonObject^> getObjectInfo(Platform::String^ fullname);
		concurrency::task<Windows::Data::Json::JsonObject^> getObjectInfo(std::queue<Platform::String^> fullnames);
		concurrency::task<commentUWPlisting> getCommentAsyncVec(Platform::String^ ID);
		concurrency::task<commentUWPlisting> getCommentAsyncVec(Platform::String^ ID, commentSort sort);
		std::shared_ptr<subredditlisting> getsubredditAsyncVec(); //assume get frontpage if no subreddit argument
		std::shared_ptr<subredditlisting> getsubredditAsyncVec(Platform::String^ subreddit);
		std::shared_ptr<subredditlisting> getsubredditAsyncVec(Platform::String^ subreddit, postSort sort, timerange range); //returns an empty vector that async populates
		Windows::Web::Http::HttpClient^ httpClient;
	};

}