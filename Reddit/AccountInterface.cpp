#include "pch.h"
#include "AccountInterface.h"
#include <sstream>
#include "globalvars.h"
#include "DelegateCommand.h"
#include "subpost.h"
#include <experimental/resumable>
#include "subpostUWP.h"
#include <pplawait.h>
#include <ppl.h>
#include "ApplicationDataHelper.h"
#include "commentUWPitem.h"
#include "MixedContentSorter.h"
using namespace Windows::Foundation;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;
namespace wsaw = Windows::Security::Authentication::Web;
using namespace account;
namespace account
{
	const std::wstring AccountInterface::apibase = L"https://oauth.reddit.com/";
	Platform::String^ AccountInterface::baseURI = ref new Platform::String(L"https://oauth.reddit.com/");
	Concurrency::task<AccountInterface*> AccountInterface::LoginNewAcc()
	{
		GUID state;
		CoCreateGuid(&state); //create unique state string to make sure that it is our authentication when it comes back
		std::wstringstream ss;
		ss << L"https://www.reddit.com/api/v1/authorize?client_id=ZPFRVRwyAE1uRA" << L"&response_type=code&state=" << std::hex << state.Data1 << state.Data2 << state.Data3 << state.Data3 << L"&redirect_uri=http://bobthefirst.net&duration=permanent&scope=creddits, modcontributors, modconfig, subscribe, wikiread, wikiedit, vote, mysubreddits, submit, modlog, modposts, modflair, save, modothers, read, privatemessages, report, identity, livemanage, account, modtraffic, edit, modwiki, modself, history, flair"; //authentication request uri
		Uri^ startURIz = ref new Uri(ref new Platform::String(ss.str().data()));
		Uri^ redirect = ref new Uri(L"http://bobthefirst.net"); //oauth redirect uri
		return concurrency::create_task(wsaw::WebAuthenticationBroker::AuthenticateAsync(wsaw::WebAuthenticationOptions::None, startURIz, redirect)).then([](wsaw::WebAuthenticationResult^ outcome) {
			switch (outcome->ResponseStatus)
			{
			case wsaw::WebAuthenticationStatus::Success: {
				WwwFormUrlDecoder urldecoder(outcome->ResponseData);
				auto code = urldecoder.GetFirstValueByName(L"code");
				Platform::Collections::Map<Platform::String^, Platform::String^>^ postcontent = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
				postcontent->Insert(L"grant_type", L"authorization_code");
				postcontent->Insert(L"code", code);
				postcontent->Insert(L"redirect_uri", L"http://bobthefirst.net");
				auto message = ref new Windows::Web::Http::HttpFormUrlEncodedContent(postcontent);
				globalvars::generalHttp->DefaultRequestHeaders->Authorization = ref new Windows::Web::Http::Headers::HttpCredentialsHeaderValue(L"basic", L"WlBGUlZSd3lBRTF1UkE6");
				return globalvars::generalHttp->PostAsync(ref new Uri(L"https://www.reddit.com/api/v1/access_token"), message);
			}break;
			case wsaw::WebAuthenticationStatus::UserCancel:
			{
				throw NewAccountLoginFailure(NewAccountLoginFailure::reason::UserCanceled);
			}break;
			case wsaw::WebAuthenticationStatus::ErrorHttp:
			{
				throw NewAccountLoginFailure(NewAccountLoginFailure::reason::networkerror, L"Http Error: " + std::to_wstring(outcome->ResponseErrorDetail));
			}
			}
		}).then([](Windows::Web::Http::HttpResponseMessage^ response) {
			globalvars::generalHttp->DefaultRequestHeaders->Authorization = nullptr;
			if (response->IsSuccessStatusCode)
			{
				return response->Content->ReadAsStringAsync();
			}
		}, concurrency::task_continuation_context::use_arbitrary()).then([](Platform::String^ jsonResponse) {
			JsonObject^ jsonContent;
			JsonObject::TryParse(jsonResponse, &jsonContent);
			auto token = jsonContent->GetNamedString(L"access_token");
			auto refreshtoken = jsonContent->GetNamedString(L"refresh_token");

			auto acc =  new AccountInterface(refreshtoken, token);
			JsonObject^ mejson;
			JsonObject::TryParse(concurrency::create_task(acc->httpClient->GetStringAsync(ref new Uri(Platform::StringReference((apibase + L"api/v1/me").data())))).get(), &mejson);
			acc->me = jsontoinfo(mejson);
			ApplicationDataHelper::userHelpers::cacheMyUser(acc->me);
			auto credvault = ref new Windows::Security::Credentials::PasswordVault();
			credvault->Add(ref new Windows::Security::Credentials::PasswordCredential("reddit_refresh_token", acc->me.username, refreshtoken));
		
			return acc;
		}, concurrency::task_continuation_context::use_arbitrary());
	}



	concurrency::task<Windows::Data::Json::JsonObject^> AccountInterface::getObjectInfo(Platform::String ^ fullname)
	{
		Platform::String^ apiendpoint =  L"api/info" + L"?id="  + fullname + L"&raw_json=1";
		return getJsonAsync(ref new Windows::Foundation::Uri(baseURI, apiendpoint));
	}

	concurrency::task<Windows::Data::Json::JsonObject^> AccountInterface::getObjectInfo(std::queue<Platform::String^> fullnames)
	{
		throw ref new Platform::NotImplementedException();
	}

	concurrency::task<void> AccountInterface::vote(Platform::IBox<bool>^ dir, Platform::String ^ id)
	{
		auto content = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
		Platform::String^ z;
		if (dir == nullptr)
			z = L"0";
		else if (dir->Value == false)
			z = L"-1";
		else if (dir->Value == true)
			z = L"1";
		content->Insert(Platform::StringReference(L"dir"), z);
		content->Insert(Platform::StringReference(L"id"), id);
		return concurrency::create_task(httpClient->PostAsync(ref new Uri(Platform::StringReference((apibase + L"api/vote").data())), ref new Windows::Web::Http::HttpFormUrlEncodedContent(content))).then([](Windows::Web::Http::HttpResponseMessage^ respond)
		{
			respond->EnsureSuccessStatusCode();
		});
	}

	concurrency::task<void> AccountInterface::giveGold(Platform::String ^ fullname)
	{
		return concurrency::create_task(httpClient->PostAsync(ref new Uri(Platform::StringReference((apibase + L"api/v1/gold/gild/").data()), fullname), nullptr)).then([](Windows::Web::Http::HttpResponseMessage^ h) {
			auto x = h->ToString();
		}, concurrency::task_continuation_context::use_arbitrary());
	}

	concurrency::task<HttpResponseMessage^> AccountInterface::comment(Platform::String ^ ID, Platform::String ^ md)
	{
		Platform::Collections::Map<Platform::String^, Platform::String^>^ z = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
		z->Insert("raw_json", "1");
		z->Insert("api_type", "json");
		z->Insert("text", md);
		z->Insert("thing_id", ID);

		return concurrency::create_task(httpClient->PostAsync(ref new Uri(baseURI, "api/comment"), ref new HttpFormUrlEncodedContent(z)));
	}


	concurrency::task<Windows::Data::Json::JsonObject^> AccountInterface::getJsonAsync(Windows::Foundation::Uri ^ requestUri)
	{
		auto debugstring = requestUri->ToString();
		return concurrency::create_task(httpClient->GetStringAsync(requestUri)).then([](Platform::String^ content) {
			return JsonObject::Parse(content);
		});
	}


	concurrency::task<std::pair<Platform::Collections::Vector<IRedditTypeIdentifier^>^,const Platform::String^>>  AccountInterface::getMessages(Messages_Where m)
	{
		Platform::String^ path = L"message";
		switch (m)
		{
		case Messages_Where::inbox:
			path += L"/inbox";
			break;
		case Messages_Where::sent:
			path += L"/sent";
			break;
		case Messages_Where::unread:
			path += L"/unread";
		}
		return getJsonFromBasePath(path).then([](Windows::Data::Json::JsonObject^ j) {
			const auto &maindata = j->GetNamedObject("data");
			const Platform::String^ after = maindata->GetNamedString("after");
			return std::make_pair(account::MixedContentSorter(maindata->GetNamedArray("children")), std::move(after));
		});
	}

	concurrency::task<void> AccountInterface::updateInfo()
	{
		
		return getJsonAsync(ref new Uri(Platform::StringReference((apibase + L"api/v1/me").data()))).then([this](Windows::Data::Json::JsonObject^ jsoninfo) {
			me =  jsontoinfo(jsoninfo);
			ApplicationDataHelper::userHelpers::cacheMyUser(me);
		});
		
	}

	AccountInfo AccountInterface::jsontoinfo(Windows::Data::Json::JsonObject ^ jsoninfo)
	{
		AccountInfo returning;
		returning.comment_karma = jsoninfo->GetNamedNumber("comment_karma");
		returning.has_mail = jsoninfo->GetNamedBoolean("has_mail");
		returning.has_gold = jsoninfo->GetNamedBoolean("is_gold");
		returning.link_karma = jsoninfo->GetNamedNumber("link_karma");
		returning.username = jsoninfo->GetNamedString("name");
		return returning;
	}

	Concurrency::task<void> AccountInterface::changeSave(Platform::String ^ fullname, bool dir)
	{
		std::wstring path = L"api/";
		if (!dir)
			path += L"un";
		path += L"save";
		return concurrency::create_task(httpClient->PostAsync(ref new Uri(baseURI, Platform::StringReference(path.data())), ref new Windows::Web::Http::HttpFormUrlEncodedContent(ref new Platform::Collections::Map<Platform::String^, Platform::String^>({ std::pair<Platform::String^, Platform::String^>("id", fullname) })))).then([](HttpResponseMessage^ xv) {xv->EnsureSuccessStatusCode(); });
	}

	concurrency::task<Windows::Data::Json::JsonObject^> AccountInterface::getJsonFromBasePath(Platform::String ^ path)
	{
		return getJsonAsync(ref new Windows::Foundation::Uri(baseURI, path));
	}

	concurrency::task<std::vector<reportReason>> AccountInterface::getRules(Platform::String ^ subreddit)
	{
		return getJsonAsync(ref new Uri(baseURI, "r/" + subreddit + "/about/rules.json?raw_json=1")).then([](Windows::Data::Json::JsonObject^ json) {

			auto jsonrulesarr = json->GetNamedArray("rules");
			std::vector<reportReason> output(jsonrulesarr->Size);
			concurrency::parallel_transform(Windows::Foundation::Collections::begin(jsonrulesarr), Windows::Foundation::Collections::end(jsonrulesarr), output.begin(), [](Windows::Data::Json::IJsonValue^ jv) {
				return account::jsonToReport(jv->GetObject());
			});
			return output;
		});
	}

	AccountInterface::AccountInterface()
	{
		httpClient = ref new Windows::Web::Http::HttpClient();
	}

	AccountInterface::AccountInterface(Platform::String ^ refresh, AccountInfo cachedInfo)
	{
		httpClient = ref new Windows::Web::Http::HttpClient(ref new authFilter(ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter(), refresh));
		httpClient->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue("Here's a user agent. You happy?"));
		me = std::move(cachedInfo);
		updateInfo();
	}

	AccountInterface::AccountInterface(Platform::String ^ refresh)
	{

		httpClient = ref new Windows::Web::Http::HttpClient(ref new authFilter(ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter(), refresh));
		httpClient->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue("Here's a user agent. You happy?"));
		updateInfo();
		//updateInfo();
	}

	AccountInterface::AccountInterface(Platform::String ^ refresh, Platform::String ^ currentauth)
	{
		httpClient = ref new Windows::Web::Http::HttpClient(ref new authFilter(ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter(), refresh, currentauth));
		httpClient->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue("Here's a user agent. You happy?"));
	}
	concurrency::task<commentUWPlisting> AccountInterface::getCommentAsyncVec(Platform::String^ ID)
	{
		return getCommentAsyncVec(ID, commentSort::default);
	}
	concurrency::task<commentUWPlisting> AccountInterface::getCommentAsyncVec(Platform::String ^ ID, commentSort sort)
	{

		std::wstring reqstr = L"/comments/";
		reqstr += ID->Data();
		reqstr += L"?raw_json=1&limit=400";
		switch (sort)
		{
		case commentSort::best:
			reqstr += L"&sort=confidence";
			break;
		case commentSort::controversial:
			reqstr += L"&sort=controversial";
			break;
		case commentSort::top:
			reqstr += L"&sort=top";
			break;
		case commentSort::New:
			reqstr += L"&sort=new";
			break;
		case commentSort::old:
			reqstr += L"&sort=old";
			break;
		case commentSort::qa:
			reqstr += L"&sort=qa";
			break;
		case commentSort::default:
			break;
		}
		return concurrency::create_task(httpClient->GetStringAsync(ref new Uri(baseURI, Platform::StringReference(reqstr.data())))).then([](Platform::String^ jsonstr) {
			try
			{
				commentUWPlisting comments;
				JsonArray^ json = Windows::Data::Json::JsonArray::Parse(jsonstr);
				//comments.parent = ref new subpostUWP(json->GetObjectAt(0));
				JsonObject^ data = json->GetObjectAt(1)->GetNamedObject("data");
				JsonValue^ tempvalue = data->GetNamedValue("after");


				JsonArray^ jsoncomments = data->GetNamedArray("children");
				auto lastObject = (*(Windows::Foundation::Collections::end(jsoncomments) - 1))->GetObject();
				if (lastObject->GetNamedString("kind") == "more")
				{
					comments.commentList = std::vector<CommentUWPitem^>(jsoncomments->Size - 1);

					comments.more = ref new moreComments(lastObject->GetNamedObject("data"));
					jsoncomments->RemoveAtEnd();
				}
				else
					comments.commentList = std::vector<CommentUWPitem^>(jsoncomments->Size);
				comments.commentList[0] = ref new CommentUWPitem(jsoncomments->GetObjectAt(0)->GetNamedObject("data"));
				if (jsoncomments->Size > 1)
				{
					concurrency::parallel_transform(Windows::Foundation::Collections::begin(jsoncomments) + 1, Windows::Foundation::Collections::end(jsoncomments), comments.commentList.begin()+1, [&sub = comments.commentList[0]->helper.parent_subreddit](Windows::Data::Json::IJsonValue^ z) {
						return ref new CommentUWPitem(z->GetObject()->GetNamedObject("data"), nullptr, true, sub);
					});
				}
				return comments;
			}
			catch (...)
			{
				__debugbreak();
			}
			
			
		},concurrency::task_continuation_context::use_arbitrary());
		//throw ref new Platform::NotImplementedException();
	}
	concurrency::task<commentUWPlisting> AccountInterface::getmorecomments(moreComments ^ more, Platform::String^ link_id, Platform::String^ parent_id)
	{
		std::wstring urlstr = L"api/morechildren?api_type=json&children=" + more->morelist + L"&link_id=" + link_id->Data();
		return getJsonAsync(ref new Windows::Foundation::Uri(baseURI, Platform::StringReference(urlstr.data()))).then([link_id, parent_id](Windows::Data::Json::JsonObject^ j) {
			auto arr = j->GetNamedObject("json")->GetNamedObject("data")->GetNamedArray("things");
			commentUWPlisting reps;
			std::vector<CommentUWPitem^> refmap;
			CommentUWPitem^ r;
			
			auto lastObject = (*(Windows::Foundation::Collections::end(arr) - 1))->GetObject();
			if (lastObject->GetNamedString("kind") == "more")
			{
				reps.more = ref new moreComments(lastObject->GetNamedObject("data"));
				arr->RemoveAtEnd();
			}
			const wchar_t* pwchar = parent_id->Data();
			for (auto &&a : arr)
			{
				auto j = a->GetObject();
				CommentUWPitem^ pItem = nullptr;
				auto data = j->GetNamedObject("data");
				auto pid = data->GetNamedString("parent_id");
				if (wcscmp(pwchar, pid->Data()+3) == 0)
				{
					pItem = nullptr;
				}
				else if (wcscmp(r->helper.id->Data(), pid->Data() + 3) == 0)
				{
					if (r->replies == nullptr)
					{
						r->replies = new commentUWPlisting;
					}
					pItem = r;
				}
				else
				{
					auto it = std::find_if(refmap.rbegin(), refmap.rend(), [pid](CommentUWPitem^ c) {
						return (wcscmp(c->helper.id->Data(), pid->Data()+3) == 0);
					});
					if (it == refmap.rend())
						__debugbreak();
					else
					{
						if ((*it)->replies == nullptr)
						{
							(*it)->replies = new commentUWPlisting;
						}
						pItem = *it;
					}
				}

				if (j->GetNamedString("kind") == "more")
				{
					try { pItem->replies->more = ref new moreComments(data); }
					catch(...)
					{
						__debugbreak();
					}
				}
				else
				{
					r = ref new CommentUWPitem(data, link_id, false);
					if (pItem == nullptr)
					{
						reps.commentList.push_back(r);
					}
					else
					{
						pItem->replies->commentList.push_back(r);
					}
					refmap.push_back(r);
				}
				
			}
			return reps;
		});
	}
	std::unique_ptr<subredditlisting> AccountInterface::getsubredditAsyncVec()
	{
		return getsubredditAsyncVec("");
	}
	std::unique_ptr<subredditlisting> AccountInterface::getsubredditAsyncVec(Platform::String ^ subreddit)
	{

		return getsubredditAsyncVec(subreddit, postSort::Defaultsort, timerange::Default);
		
	}
	std::unique_ptr<subredditlisting> AccountInterface::getsubredditAsyncVec(Platform::String ^ subreddit, postSort sort, timerange range)
	{
		if (subreddit != "")
		{
			subreddit = L"r/" + subreddit + L"/";
		}
		std::wstring urlstr = subreddit->Data();
		switch (sort)
		{
		case postSort::Defaultsort:
			break;
		case postSort::controversial:
			urlstr += L"controversial/";
			break;
		case postSort::hot:
			urlstr += L"hot/";
			break;
		case postSort::New:
			urlstr += L"new/";
			break;
		case postSort::rising:
			urlstr += L"rising/";
			break;
		case postSort::top:
			urlstr += L"top/";
			break;
	
		default:
			__assume(0);
		}
		urlstr += L"?raw_json=1&limit=40";
		switch (range)
		{
		case timerange::Default:
			break;
		case timerange::all:
			urlstr += L"&t=all";
			break;
		case timerange::day:
			urlstr += L"&t=day";
			break;
		case timerange::hour:
			urlstr += L"&t=hour";
			break;
		case timerange::month:
			urlstr += L"&t=month";
			break;
		case timerange::week:
			urlstr += L"&t=week";
			break;
		case timerange::year:
			urlstr += L"&t=year";
			break;
		default:
			__assume(0);
		}
		subredditlisting*listing = new subredditlisting();
		listing->listing = ref new Platform::Collections::Vector<subpostUWP^>();
		
		listing->getTask = getJsonAsync(ref new Uri(baseURI, Platform::StringReference(urlstr.data()))).then([listing](Windows::Data::Json::JsonObject^ json) {
			auto data = json->GetNamedObject("data");
			try
			{
				auto v = data->GetNamedValue("before");
				if (v->ValueType == Windows::Data::Json::JsonValueType::String)
				{
					listing->before = v->GetString();
				}
				else
				{
					listing->before = nullptr;
				}
			}
			catch (...)
			{
				listing->before = nullptr;
			}
			try
			{
				auto v = data->GetNamedValue("after");
				if (v->ValueType == Windows::Data::Json::JsonValueType::String)
				{
					listing->after = v->GetString();
				}
				else
				{
					listing->after = nullptr;
				}
			}
			catch (...)
			{
				listing->after = nullptr;
			}
			
			auto jsonPosts = data->GetNamedArray("children");
			
			for (auto x : jsonPosts)
			{
				auto jsonpost = x->GetObject()->GetNamedObject("data");
				subpostUWP^ addingpost= ref new subpostUWP(jsonpost);
				listing->listing->Append(addingpost);
				
			}
			return listing->listing;
		}), concurrency::task_continuation_context::use_arbitrary;
		return std::unique_ptr<subredditlisting>(listing);
	}

	concurrency::task<Platform::Collections::Vector<IRedditTypeIdentifier^>^> AccountInterface::getDomain(Platform::String ^ domain)
	{
		getJsonFromBasePath(L"domain/" + domain + L"?raw_json=1").then([](Windows::Data::Json::JsonObject^ j) {

		});
		throw ref new Platform::NotImplementedException();
	}

	concurrency::task<Platform::Collections::Vector<IRedditTypeIdentifier^>^> AccountInterface::getMixedCollection(Platform::String ^ endpoint)
	{
		return getJsonAsync(ref new Windows::Foundation::Uri(baseURI, endpoint)).then([](Windows::Data::Json::JsonObject^ json) {
			const auto & arr = json->GetNamedObject("data")->GetNamedArray("children");
			auto vec = ref new Platform::Collections::Vector<IRedditTypeIdentifier^>(arr->Size);
			concurrency::parallel_transform(Windows::Foundation::Collections::begin(arr), Windows::Foundation::Collections::end(arr), Windows::Foundation::Collections::begin(vec), [](Windows::Data::Json::IJsonValue^ v) {
				return ambiguousJsonResolver(v->GetObject());
			});
			return vec;
		});
	}



	
	IRedditTypeIdentifier ^ AccountInterface::ambiguousJsonResolver(Windows::Data::Json::JsonObject ^ j)
	{
		try {
			auto kind = j->GetNamedString("kind")->Data();
			if (*kind == L't')
			{
				switch (*(kind + 1))
				{
				case L'1':
				{
					auto ret = ref new CommentUWPitem(j->GetNamedObject("data"), nullptr, false);
					ret->cacheMDElements();
					return ret;
				}break;
				case L'2':
				{

				}break;
				case L'3':
				{
					return ref new subpostUWP(j->GetNamedObject("data"));
				}break;
				case L'4':
				{

				}break;
				case L'5':
				{

				}break;
				case L'6':
				{

				}break;
				default:
					__assume(0);
				}
			}
			else
			{
				__debugbreak();
			}
		}
		catch (Platform::Exception^ e)
		{
			__debugbreak();
		}
	}
	

	AccountInterface::authFilter::authFilter(Windows::Web::Http::Filters::IHttpFilter ^ innerFilter, Platform::String ^ refreshtoken)
	{
		this->refreshToken = refreshtoken;
		//refreshauth();
		this->innerFilter = innerFilter;
	}

	AccountInterface::authFilter::authFilter(Windows::Web::Http::Filters::IHttpFilter ^ innerFilter, Platform::String ^ refreshtoken, Platform::String ^ currentToken)
	{
		this->refreshToken = refreshtoken;
		this->innerFilter = innerFilter;
		AuthHeader = ref new Windows::Web::Http::Headers::HttpCredentialsHeaderValue(L"bearer", currentToken);
	}

	Windows::Foundation::IAsyncOperationWithProgress<Windows::Web::Http::HttpResponseMessage^, Windows::Web::Http::HttpProgress>^ AccountInterface::authFilter::SendRequestAsync(Windows::Web::Http::HttpRequestMessage ^ request)
	{
		
		return concurrency::create_async([=](concurrency::progress_reporter<HttpProgress> reporter, concurrency::cancellation_token token)
		{


			unsigned int retries = 0;
			if (request->Properties->HasKey(L"retries"))
			{
				retries = static_cast<unsigned int>(request->Properties->Lookup(L"retries"));
			}
			
			if (AuthHeader == nullptr)
			{
				return refreshauth().then([token,this, request](void){
					//IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ operation = innerFilter->SendRequestAsync(request);
					//operation->Progress = ref new AsyncOperationProgressHandler<HttpResponseMessage^, HttpProgress>([reporter, &retries](
					//	IAsyncOperationWithProgress<HttpResponseMessage^, HttpProgress>^ asyncInfo,
					//	HttpProgress progress)
					//{
					//	progress.Retries += retries;
					//	//reporter.report(progress);
					//});
					request->Headers->Authorization = AuthHeader;
					Platform::String^ eige = request->Headers->Authorization->ToString();
					return concurrency::create_task(innerFilter->SendRequestAsync(request), token);
				});
			}
			else
			{
			
				request->Headers->Authorization = AuthHeader;
				return concurrency::create_task(innerFilter->SendRequestAsync(request)).then([=](concurrency::task<HttpResponseMessage^> sendRequestTask)
				{
					HttpResponseMessage^ response = sendRequestTask.get();
					//throw ref new Platform::NotImplementedException("need to figure out which status code for refresh");
					if (response->StatusCode == Windows::Web::Http::HttpStatusCode::Forbidden)
					{
						request->Headers->Authorization = AuthHeader;
						request->Properties->Insert("retries", (retries + 1));
						return refreshauth().then([=]() {
							return create_task(SendRequestAsync(request), token);
						});
					}
					else return sendRequestTask;
				});
			}
		});
	}

	concurrency::task<void> AccountInterface::authFilter::refreshauth()
	{
		globalvars::generalHttp->DefaultRequestHeaders->Authorization = ref new Windows::Web::Http::Headers::HttpCredentialsHeaderValue(L"basic", L"WlBGUlZSd3lBRTF1UkE6");
		Platform::Collections::Map<Platform::String^, Platform::String^>^ postcontent = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
		postcontent->Insert(L"grant_type", L"refresh_token");
		postcontent->Insert(L"refresh_token", refreshToken);
		return concurrency::create_task(globalvars::generalHttp->PostAsync(ref new Uri(L"https://www.reddit.com/api/v1/access_token"), ref new Windows::Web::Http::HttpFormUrlEncodedContent(postcontent))).then([this](HttpResponseMessage^ res) {
			Platform::String^ debugline = res->Content->ToString();
			if (res->IsSuccessStatusCode)
			{
				auto currentToken = JsonObject::Parse(res->Content->ToString())->GetNamedString("access_token");
				AuthHeader = ref new Windows::Web::Http::Headers::HttpCredentialsHeaderValue(L"bearer", currentToken);
			}
		});

	}

}