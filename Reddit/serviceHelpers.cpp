#include "pch.h"
#include "serviceHelpers.h"
#include "globalvars.h"
#include <regex>
#include <ppl.h>
#include <concurrent_queue.h>
#include <concurrent_vector.h>
using namespace account;
using namespace account::serviceHelpers;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;
typedef concurrency::task<std::pair<account::postContentType, previewHelperbase*>>(*concurrentHelper) (std::variant<Windows::Data::Json::JsonObject^, Windows::Foundation::Uri^>);
typedef std::pair<account::postContentType, previewHelperbase*>(*straightHelper)(std::variant<Windows::Data::Json::JsonObject^, Windows::Foundation::Uri^>);

#define _SERVICE_HELPER_FUNC_SIG_C(xii) concurrency::task<std::pair<account::postContentType, previewHelperbase*>> xii (std::variant<Windows::Data::Json::JsonObject^, Windows::Foundation::Uri^> v)
#define _SERVICE_HELPER_FUNC_SIG_S(xii) std::pair<account::postContentType, previewHelperbase*> xii (std::variant<Windows::Data::Json::JsonObject^, Windows::Foundation::Uri^> v)
_SERVICE_HELPER_FUNC_SIG_C(imgurHelper);
_SERVICE_HELPER_FUNC_SIG_S(iRedditHelper);
_SERVICE_HELPER_FUNC_SIG_C(gfycatHelper);
_SERVICE_HELPER_FUNC_SIG_C(streamableHelper);
const
std::unordered_map<
	std::wstring,
	std::variant<
	concurrentHelper,
	straightHelper
	>
> domainMap = { { L"imgur.com" , &imgurHelper }, {L"i.imgur.com", &imgurHelper} , {L"i.redd.it", &iRedditHelper}, {L"gfycat.com", &gfycatHelper}, {L"streamable.com" , &streamableHelper} };

concurrency::task<std::pair<account::postContentType, previewHelperbase*>> serviceHelpers::jsonHelper(Windows::Data::Json::JsonObject^ z)
{
	return concurrency::create_task([z]() {
		try
		{

			auto url = ref new Windows::Foundation::Uri(z->GetNamedString("url"));
			
			auto td = z->GetNamedValue("media");
			if (td->ValueType != Windows::Data::Json::JsonValueType::Null)
			{
				if (td->GetObject()->HasKey("reddit_video"))
				{
					auto reddVideo = td->GetObject()->GetNamedObject("reddit_video");
					auto hlsUrl = ref new Windows::Foundation::Uri(reddVideo->GetNamedString("hls_url"));
					if (reddVideo->GetNamedBoolean("is_gif"))
					{
						return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>(account::postContentType::giftype, new gifvDisplay(hlsUrl, url)));
					}
					else
					{

						return concurrency::create_task(Windows::Media::Streaming::Adaptive::AdaptiveMediaSource::CreateFromUriAsync(hlsUrl)).then([](Windows::Media::Streaming::Adaptive::AdaptiveMediaSourceCreationResult^ res) {

							if (res->Status == Windows::Media::Streaming::Adaptive::AdaptiveMediaSourceCreationStatus::Success)
							{
								res->MediaSource->InitialBitrate = *std::max_element(Windows::Foundation::Collections::begin(res->MediaSource->AvailableBitrates), Windows::Foundation::Collections::end(res->MediaSource->AvailableBitrates));
								return res->MediaSource;
							}
							else
							{
								res->HttpResponseMessage->EnsureSuccessStatusCode();

								throw ref new Platform::Exception(res->ExtendedError.Value, res->Status.ToString());
							}

						}).then([hlsUrl, url](concurrency::task<Windows::Media::Streaming::Adaptive::AdaptiveMediaSource^> z) {
							try { return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::videotype, new videoDisplay(hlsUrl, url, z.get())); }
							catch (Platform::Exception^ e)
							{
								__debugbreak();
							}
							catch (...)
							{
								__debugbreak();
							}
						});
					}
				}
				else if (td->GetObject()->HasKey("oembed"))
				{

				}
			}
				std::unordered_map<std::wstring,
					std::variant<
					concurrentHelper,
					straightHelper
					>>::const_iterator it = domainMap.find(z->GetNamedString("domain")->Data());
				if (it == domainMap.end())
					return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>(postContentType::linktype, new previewHelperbase(url)));
				else
				{
					
					if (std::holds_alternative<concurrentHelper>(it->second))
					{
						return std::get<concurrentHelper>(it->second)(z);
					}
					else
					{
						return concurrency::task_from_result(std::get<straightHelper>(it->second)(z));

					}
				}
			
		}
		catch (Platform::Exception^ e)
		{
			__debugbreak();
		}

	});

}

concurrency::task<std::pair<account::postContentType, previewHelperbase*>> account::serviceHelpers::urlHelper(Windows::Foundation::Uri ^ url)
{
	return concurrency::create_task([url] {
		auto it = domainMap.find(url->Domain->Data());
		if (it != domainMap.end())
		{
			if (std::holds_alternative<concurrentHelper>(it->second))
				return std::get<concurrentHelper>(it->second)(url);
			else
				return concurrency::task_from_result<std::pair<account::postContentType, previewHelperbase*>>(std::get<straightHelper>(it->second)(url));
		}
		else
		{
			return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>(account::postContentType::linktype, new previewHelperbase(url)));
		}
	});

}

_SERVICE_HELPER_FUNC_SIG_C(imgurHelper)
{

	Windows::Foundation::Uri^ url;

	if (std::holds_alternative<Windows::Foundation::Uri^>(v))
	{
		url = std::get<Windows::Foundation::Uri^>(v);
	}
	else
	{
		url = ref new Windows::Foundation::Uri(std::get<Windows::Data::Json::JsonObject^>(v)->GetNamedString("url"));
		if (std::get<Windows::Data::Json::JsonObject^>(v)->GetNamedString("post_hint") == L"image")
		{
			return concurrency::task_from_result<std::pair<account::postContentType, previewHelperbase*>>(std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, new imageDisplay(url)));
		}

	}
	return concurrency::create_task([url, v] {
		try
		{
			std::wstring id = url->Path->Data();
			const std::wregex remove{ L"[^/]*$" };
			std::wsmatch result;
			std::regex_search(id, result, remove);
			id = result.str();
			size_t lastindex = id.find_last_of(L"."); //get rid of extension
			if (lastindex != std::wstring::npos)
				id = id.substr(0, lastindex);
			bool success = false;
			auto imageEndpoint = [url, id]()
			{
				return concurrency::create_task(globalvars::imgurHttp->GetAsync(ref new Windows::Foundation::Uri("https://api.imgur.com/3/image/", Platform::StringReference(id.data())))).then([url](Windows::Web::Http::HttpResponseMessage^ z) {
					Platform::String^ x = z->Content->ToString();


					z->EnsureSuccessStatusCode();
					auto json = Windows::Data::Json::JsonObject::Parse(z->Content->ToString())->GetNamedObject("data");
					if (json->GetNamedBoolean("animated"))
					{
						return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::giftype, new gifvDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("mp4")), url));
					}
					else
					{
						return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, new imageDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("link")), url));
					}

				});
			};
			auto albumEndpoint = [url,id]()
			{
				return concurrency::create_task(globalvars::imgurHttp->GetAsync(ref new Windows::Foundation::Uri("https://api.imgur.com/3/album/", Platform::StringReference(id.data())))).then([url](Windows::Web::Http::HttpResponseMessage^ z) {

					try
					{
						z->EnsureSuccessStatusCode();
						auto json = Windows::Data::Json::JsonObject::Parse(z->Content->ToString())->GetNamedObject("data");
						auto arrazy = json->GetNamedArray("images");
						Platform::Collections::Vector<Windows::Foundation::Uri^>^ vec = ref new Platform::Collections::Vector<Windows::Foundation::Uri^>(arrazy->Size);
						for (auto &&a : arrazy)
						{
							vec->Append(ref new Windows::Foundation::Uri(a->GetObject()->GetNamedString("link")));
						}
						/*std::transform(Windows::Foundation::Collections::begin(arrazy), Windows::Foundation::Collections::end(arrazy), Windows::Foundation::Collections::begin(vec), [](Windows::Data::Json::IJsonValue^ vz) {
						return ref new Windows::Foundation::Uri(static_cast<Windows::Data::Json::JsonObject^>(vz)->GetNamedString("link"));
						});*/
						return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::albumtype, new albumDisplay(url, vec));
					}
					catch (...)
					{
						__debugbreak();
					}
				});
			};

			//query the image endpoint first if we suspect that it is the right one
			if (true)
				return imageEndpoint().then([albumEndpoint](concurrency::task<std::pair<account::postContentType, previewHelperbase*>> imageTask) {
				try
				{
					imageTask.wait();
					return imageTask;
				}
				catch (...)
				{
					return albumEndpoint();
				}
			}).then([](concurrency::task<std::pair<account::postContentType, previewHelperbase*>> albumTask) {
				try
				{
					albumTask.wait();
					return albumTask;
				}
				catch (...)
				{
					__debugbreak();
				}
			});

		}
		catch (...)
		{
			__debugbreak();
		}
	});
}
_SERVICE_HELPER_FUNC_SIG_S(iRedditHelper) {
	Windows::Foundation::Uri^ url;
	if (std::holds_alternative<Windows::Foundation::Uri^>(v))
	{
		url = std::get<Windows::Foundation::Uri^>(v);
	}
	else
	{
		url = ref new Windows::Foundation::Uri(std::get<Windows::Data::Json::JsonObject^>(v)->GetNamedString("url"));
	}
	account::postContentType type;
		if (url->Extension == "gif")
		{
			type = account::postContentType::giftype;
		}
		else
			type = account::postContentType::imagetype;
		return std::pair<account::postContentType, previewHelperbase*>(type, new imageDisplay(url));
}
_SERVICE_HELPER_FUNC_SIG_C(gfycatHelper)
{
	Windows::Foundation::Uri^ url;
	if (std::holds_alternative<Windows::Foundation::Uri^>(v))
	{
		url = std::get<Windows::Foundation::Uri^>(v);
	}
	else
	{
		url = ref new Windows::Foundation::Uri(std::get<Windows::Data::Json::JsonObject^>(v)->GetNamedString("url"));
	}
	auto requestUri = ref new Windows::Foundation::Uri(L"https://gfycat.com/cajax/get" + url->Path);
		auto te = requestUri->ToString();
		return concurrency::create_task(globalvars::generalHttp->GetAsync(requestUri)).then([url](Windows::Web::Http::HttpResponseMessage^ s){
			Platform::String^ jsonstr = s->Content->ToString();

			try{
				s->EnsureSuccessStatusCode();
				auto js = Windows::Data::Json::JsonObject::Parse(s->Content->ToString());
				auto xx = js->GetNamedObject("gfyItem");
				auto vv = xx->GetNamedString("mp4Url");
				return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::giftype, new gifvDisplay(ref new Windows::Foundation::Uri(vv), url));
			}
			catch (Platform::Exception^ e)
			{
				__debugbreak();
			}
			
		});
}
_SERVICE_HELPER_FUNC_SIG_C(streamableHelper)
{
	Windows::Foundation::Uri^ url;
	if (std::holds_alternative<Windows::Foundation::Uri^>(v))
	{
		url = std::get<Windows::Foundation::Uri^>(v);
	}
	else
	{
		url = ref new Windows::Foundation::Uri(std::get<Windows::Data::Json::JsonObject^>(v)->GetNamedString("url"));
	}
	auto reqURI = ref new Windows::Foundation::Uri(L"https://api.streamable.com/videos" + url->Path);
		auto xxxieg = reqURI->ToString();
	/*	Windows::Web::Http::HttpRequestMessage^ httpRequest = ref new Windows::Web::Http::HttpRequestMessage(Windows::Web::Http::HttpMethod::Get, reqURI);
		static auto auth = ref new Windows::Web::Http::Headers::HttpCredentialsHeaderValue("basic", L"cmVkZGl0Y2xpZW50ZGV2QG91dGxvb2suY29tOjEyMzQ1Nk9vIQ==");
		httpRequest->Headers->Authorization = auth;
		concurrency::create_task(globalvars::generalHttp->SendRequestAsync(httpRequest)).then([](Windows::Web::Http::HttpResponseMessage^ m) {
			m->EnsureSuccessStatusCode();
			auto json = Windows::Data::Json::JsonObject::Parse(m->Content->ToString());
		});*/
		return concurrency::create_task(globalvars::generalHttp->GetAsync(reqURI)).then([url](Windows::Web::Http::HttpResponseMessage^ x) {
			try {
				Platform::String^ z = x->ToString();
			x->EnsureSuccessStatusCode();

			auto directURI = ref new Windows::Foundation::Uri("http://",JsonObject::Parse(x->Content->ToString())->GetNamedObject("files")->GetNamedObject("mp4")->GetNamedString("url"));
			return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::videotype, new videoDisplay(directURI, url));
			}
			catch (...)
			{
				__debugbreak();
			}
		});
}

//std::vector<std::unique_ptr<previewHelperbase>> serviceHelpers::getManyHelpers(std::vector<Windows::Data::Json::JsonObject^>const & vec)
//{
	//concurrency::create_task([&vec]() {
	//	concurrency::concurrent_vector<std::pair<unsigned int, Windows::Data::Json::JsonObject^>> imgurVec, gfycatVec, httpHeadVec;

	//	std::map<unsigned int, Windows::Data::Json::JsonObject^> mymap;
	//	for (unsigned int x = 0; x < vec.size(); x++)
	//	{
	//		mymap.emplace(x, vec[x]);
	//	}
	//	std::mutex m;
	//	std::map<unsigned int, std::unique_ptr<previewHelperbase>> resultMap;
	//	concurrency::parallel_for_each(mymap.begin(), mymap.end(), [&imgurVec, &gfycatVec, &httpHeadVec, &resultMap, &m](std::pair<unsigned int, Windows::Data::Json::JsonObject^> x) {
	//		auto domain = x.second->GetNamedString("domain");
	//		if (domain == L"i.redd.it")
	//		{
	//			m.lock();
	//			resultMap.emplace(x.first, std::make_unique<imageDisplay>(ref new Windows::Foundation::Uri(x.second->GetNamedString("url"))));
	//			m.unlock();
	//		}
	//		if (domain == "imgur.com" || domain == "i.imgur.com")
	//			imgurVec.push_back(x);
	//	});
	//	if (imgurVec.size() > 0)
	//	{
	//		concurrency::concurrent_vector<std::pair<unsigned int, std::wstring>> imageEndpointQuery;
	//		concurrency::parallel_for_each(imgurVec.begin(), imgurVec.end(), [&imageEndpointQuery, &m, &resultMap](std::pair<unsigned int, Windows::Data::Json::JsonObject^> x) {
	//			auto url = ref new Windows::Foundation::Uri(x.second->GetNamedString("url"));
	//			if (x.second->GetNamedString("post_hint") == L"image")
	//			{
	//				m.lock();
	//				resultMap.emplace(x.first, std::make_unique<imageDisplay>(url));
	//				m.unlock();
	//			}
	//			else
	//			{
	//				std::wstring id = url->Path->Data();
	//				const std::wregex remove{ L"[^/]*$" };
	//				std::wsmatch result;
	//				std::regex_search(id, result, remove);
	//				id = result.str();
	//				size_t lastindex = id.find_last_of(L"."); //get rid of extension
	//				if (lastindex != std::wstring::npos)
	//					id = id.substr(0, lastindex);
	//				imageEndpointQuery.push_back(std::make_pair(x.first, std::move(id)));
	//			}
	//		});
	//		std::wstring commaList;
	//		for (auto x : imageEndpointQuery)
	//		{
	//			commaList += x.second;
	//			commaList += L',';
	//		}
	//		auto imageEndpoint = [commaList]()
	//		{
	//			return concurrency::create_task(globalvars::imgurHttp->GetAsync(ref new Windows::Foundation::Uri("https://api.imgur.com/3/image/", Platform::StringReference(commaList.data())))).then([](Windows::Web::Http::HttpResponseMessage^ z) {
	//				Platform::String^ x = z->Content->ToString();
	//				z->EnsureSuccessStatusCode();
	//				try
	//				{
	//					auto json = Windows::Data::Json::JsonObject::Parse(z->Content->ToString())->GetNamedObject("data");
	//					if (json->GetNamedBoolean("animated"))
	//					{
	//						return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::giftype, new gifvDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("link")), url));
	//					}
	//					else
	//					{
	//						return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, new imageDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("link")), url));
	//					}
	//				}
	//				catch (...)
	//				{
	//					__debugbreak();
	//				}
	//			});
	//		};
	//	}
	//});
//}

account::serviceHelpers::gifvDisplay::gifvDisplay(Windows::Foundation::Uri ^ imageDirectLink, Windows::Foundation::Uri ^ Url, bool setSource) : imageDisplay(imageDirectLink, Url)
{
	if(setSource)
		source = Windows::Media::Core::MediaSource::CreateFromUri(contentLink);
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::gifvDisplay::viewerControl()
{

	auto element = ref new Windows::UI::Xaml::Controls::MediaPlayerElement();
	element->Source = this->source;
	element->MediaPlayer->IsLoopingEnabled = true;
	element->AutoPlay = true;
	element->AreTransportControlsEnabled = false;
	element->Stretch = Windows::UI::Xaml::Media::Stretch::Uniform;
	return element;
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::previewHelperbase::viewerControl()
{
	auto webview = ref new Windows::UI::Xaml::Controls::WebView();
	webview->Navigate(Link);
	webview->Height = Windows::UI::Xaml::Window::Current->Bounds.Height - 200;
	webview->Width = Windows::UI::Xaml::Window::Current->Bounds.Width - 200;
	return webview;
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::selfDisplay::viewerControl()
{
	auto richblock = ref new Windows::UI::Xaml::Controls::RichTextBlock();
	mdblock::mdRichProperties::SetMD(richblock, md);
	//html2xaml::Properties::SetHtml(richblock, html);
	return richblock;
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::imageDisplay::viewerControl()
{
	auto img = ref new Windows::UI::Xaml::Controls::Image();
	img->Source = fullsizeImageSource();
	return img;
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::albumDisplay::viewerControl()
{
	auto flipview = ref new Windows::UI::Xaml::Controls::FlipView();
	flipview->ItemsSource = source;
	return flipview;
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::embedDisplay::viewerControl()
{
	auto webview = ref new Windows::UI::Xaml::Controls::WebView(Windows::UI::Xaml::Controls::WebViewExecutionMode::SeparateThread);
	webview->NavigateToString(framestr);
	return webview;
}

account::serviceHelpers::videoDisplay::videoDisplay(Windows::Foundation::Uri ^ imageDirectLink, Windows::Foundation::Uri ^ Url) : gifvDisplay(imageDirectLink, Url, true) , adaptive(false)
{
}

account::serviceHelpers::videoDisplay::videoDisplay(Windows::Foundation::Uri ^ imageDirectLink, Windows::Foundation::Uri ^ Url, Windows::Media::Streaming::Adaptive::AdaptiveMediaSource ^ source) : gifvDisplay(imageDirectLink, Url, false), adaptive(true)
{
	this->source = Windows::Media::Core::MediaSource::CreateFromAdaptiveMediaSource(source);
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::videoDisplay::viewerControl()
{
	
	auto player = ref new Windows::Media::Playback::MediaPlayer();
	player->AutoPlay = true;
	player->Source = source;
	auto element = ref new Windows::UI::Xaml::Controls::MediaPlayerElement();
	element->SetMediaPlayer(player);
	element->AreTransportControlsEnabled = true;
	return element;
}







//if (wcscmp(L"i.redd.it", domain) == 0)
//{
//	
//}
//else if ( (wcscmp(L"imgur.com", domain) == 0) || (wcscmp(L"i.imgur.com", domain) == 0) )
//{
//	try
//	{
//		if (z->GetNamedString("post_hint") == L"image")
//		{
//			return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, new imageDisplay(url));
//		}
//		else
//		{

//			std::wstring id = url->Path->Data();
//			const std::wregex remove{ L"[^/]*$" };
//			std::wsmatch result;
//			std::regex_search(id, result, remove);
//			id = result.str();
//			size_t lastindex = id.find_last_of(L"."); //get rid of extension
//			if (lastindex != std::wstring::npos)
//				id = id.substr(0, lastindex);
//			bool success = false;
//			auto imageEndpoint = [url, id]()
//			{
//				return concurrency::create_task(globalvars::imgurHttp->GetAsync(ref new Windows::Foundation::Uri("https://api.imgur.com/3/image/", Platform::StringReference(id.data())))).then([url](Windows::Web::Http::HttpResponseMessage^ z) {
//					Platform::String^ x = z->Content->ToString();


//						z->EnsureSuccessStatusCode();
//						auto json = Windows::Data::Json::JsonObject::Parse(z->Content->ToString())->GetNamedObject("data");
//						if (json->GetNamedBoolean("animated"))
//						{
//							return std::pair<account::postContentType, previewHelperbase*> (account::postContentType::giftype, new gifvDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("mp4")), url));
//						}
//						else
//						{
//							return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, new imageDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("link")), url));
//						}
//					
//				});
//			};
//			auto albumEndpoint = [url, id]()
//			{
//				return concurrency::create_task(globalvars::imgurHttp->GetAsync(ref new Windows::Foundation::Uri("https://api.imgur.com/3/album/", Platform::StringReference(id.data())))).then([url](Windows::Web::Http::HttpResponseMessage^ z) {
//					
//					try
//					{
//						z->EnsureSuccessStatusCode();
//						auto json = Windows::Data::Json::JsonObject::Parse(z->Content->ToString())->GetNamedObject("data");
//						auto arrazy = json->GetNamedArray("images");
//						Platform::Collections::Vector<Windows::Foundation::Uri^>^ vec = ref new Platform::Collections::Vector<Windows::Foundation::Uri^>(arrazy->Size);
//						for (auto &&a : arrazy)
//						{
//							vec->Append(ref new Windows::Foundation::Uri(a->GetObject()->GetNamedString("link")));
//						}
//						/*std::transform(Windows::Foundation::Collections::begin(arrazy), Windows::Foundation::Collections::end(arrazy), Windows::Foundation::Collections::begin(vec), [](Windows::Data::Json::IJsonValue^ vz) {
//							return ref new Windows::Foundation::Uri(static_cast<Windows::Data::Json::JsonObject^>(vz)->GetNamedString("link"));
//						});*/
//						return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::albumtype, new albumDisplay(url, vec));
//					}
//					catch(...)
//					{__debugbreak(); }
//				});
//			};

//			//query the image endpoint first if we suspect that it is the right one
//			if (true)
//				return imageEndpoint().then([albumEndpoint](concurrency::task<std::pair<account::postContentType, previewHelperbase*>> imageTask) {
//				try
//				{
//					imageTask.wait();
//					return imageTask;
//				}
//				catch (...)
//				{
//					return albumEndpoint();
//				}
//			}).then([](concurrency::task<std::pair<account::postContentType, previewHelperbase*>> albumTask) {
//				try
//				{
//					albumTask.wait();
//					return albumTask;
//				}
//				catch (...)
//				{
//					__debugbreak();
//				}
//			}).get();
//		}
//	}
//	catch (...)
//	{
//		__debugbreak();
//		return excepted();
//	}
//}
//else if (wcscmp(L"gfycat.com",domain) == 0)
//{
//	auto requestUri = ref new Windows::Foundation::Uri(L"https://gfycat.com/cajax/get"+  url->Path);
//	auto te = requestUri->ToString();
//	return concurrency::create_task(globalvars::generalHttp->GetAsync(requestUri)).then([url, excepted](Windows::Web::Http::HttpResponseMessage^ s){
//		Platform::String^ jsonstr = s->Content->ToString();

//		try{
//			s->EnsureSuccessStatusCode();
//			auto js = Windows::Data::Json::JsonObject::Parse(s->Content->ToString());
//			auto xx = js->GetNamedObject("gfyItem");
//			auto vv = xx->GetNamedString("mp4Url");
//			return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::giftype, new gifvDisplay(ref new Windows::Foundation::Uri(vv), url));
//		}
//		catch (Platform::Exception^ e)
//		{
//			__debugbreak();
//			return excepted();
//		}
//		
//	}).get();

//}
//else if (wcscmp(L"streamable.com", domain) ==0)
//{
//	auto reqURI = ref new Windows::Foundation::Uri(L"https://api.streamable.com/videos" + url->Path);
//	auto xxxieg = reqURI->ToString();
///*	Windows::Web::Http::HttpRequestMessage^ httpRequest = ref new Windows::Web::Http::HttpRequestMessage(Windows::Web::Http::HttpMethod::Get, reqURI);
//	static auto auth = ref new Windows::Web::Http::Headers::HttpCredentialsHeaderValue("basic", L"cmVkZGl0Y2xpZW50ZGV2QG91dGxvb2suY29tOjEyMzQ1Nk9vIQ==");
//	httpRequest->Headers->Authorization = auth;
//	concurrency::create_task(globalvars::generalHttp->SendRequestAsync(httpRequest)).then([](Windows::Web::Http::HttpResponseMessage^ m) {
//		m->EnsureSuccessStatusCode();
//		auto json = Windows::Data::Json::JsonObject::Parse(m->Content->ToString());
//	});*/
//	return concurrency::create_task(globalvars::generalHttp->GetAsync(reqURI)).then([url, excepted](Windows::Web::Http::HttpResponseMessage^ x) {
//		try {
//			Platform::String^ z = x->ToString();
//		x->EnsureSuccessStatusCode();

//		auto directURI = ref new Windows::Foundation::Uri("http://",JsonObject::Parse(x->Content->ToString())->GetNamedObject("files")->GetNamedObject("mp4")->GetNamedString("url"));
//		return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::videotype, new videoDisplay(directURI, url));
//		}
//		catch (...)
//		{
//			return excepted();
//		}
//	}).get();
//}
//else
//{
//	return excepted();
//}