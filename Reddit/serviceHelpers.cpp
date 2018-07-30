#include "pch.h"
#include "serviceHelpers.h"
#include "globalvars.h"
#include <regex>
#include <ppl.h>
#include "subpostUWP.h"
#include <concurrent_queue.h>
#include <concurrent_vector.h>
using namespace account;
using namespace account::serviceHelpers;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;
#define urioperator concurrency::task<std::pair<account::postContentType, previewHelperbase*>> operator()(Windows::Foundation::Uri^ uri, account::postContentType hint = account::postContentType::selftype) const
#define jsonoperator concurrency::task<std::pair<account::postContentType, previewHelperbase*>> operator()(Windows::Data::Json::JsonObject^ json, account::postContentType hint = account::postContentType::selftype) const
#define puricall std::pair<account::postContentType, previewHelperbase*> prelimCall(Windows::Foundation::Uri^ uri) const
#define pjsoncall std::pair<account::postContentType, previewHelperbase*> prelimCall(Windows::Data::Json::JsonObject^ json) const
//typedef concurrency::task<std::pair<account::postContentType, previewHelperbase*>>(*concurrentHelper) (std::variant<Windows::Data::Json::JsonObject^, Windows::Foundation::Uri^>);
typedef std::pair<account::postContentType, previewHelperbase*>(*straightHelper)(std::variant<Windows::Data::Json::JsonObject^, Windows::Foundation::Uri^>);

//#define _SERVICE_HELPER_FUNC_SIG_C(xii) concurrency::task<std::pair<account::postContentType, previewHelperbase*>> xii (std::variant<Windows::Data::Json::JsonObject^, Windows::Foundation::Uri^> v)
//#define _SERVICE_HELPER_FUNC_SIG_S(xii) std::pair<account::postContentType, previewHelperbase*> xii (std::variant<Windows::Data::Json::JsonObject^, Windows::Foundation::Uri^> v)
namespace account::serviceHelpers
{
	struct CrossPostDisplay final : previewHelperbase
	{
	private:
		Platform::WeakReference ParentPost;
	public:
		CrossPostDisplay(account::subpostUWP^ s) : previewHelperbase(s->link)
		{
			easypreview = true;
			ParentPost = s;
		}
		virtual Windows::UI::Xaml::UIElement^ viewerControl();
	};
}
concurrency::task<std::pair<account::postContentType, previewHelperbase*>> serviceHelpers::jsonHelper(Windows::Data::Json::JsonObject^ z)
{
	return concurrency::create_task([z]() {
		if (z->HasKey("crosspost_parent"))
		{
			return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>(postContentType::xpost, new CrossPostDisplay(ref new account::subpostUWP(z->GetNamedArray("crosspost_parent_list")->GetObjectAt(0)))));
		}
		else
		{
			auto url = ref new Windows::Foundation::Uri(z->GetNamedString("url"));
			try
			{
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
				auto it = domainMap.find(std::wstring_view(z->GetNamedString("domain")->Data()));
				if (it == domainMap.end())
					return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>(postContentType::linktype, new previewHelperbase(url)));
				else
				{
					return it->second(z);
				}

			}
			catch (Platform::Exception^ e)
			{
				__debugbreak();
				return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>(postContentType::linktype, new previewHelperbase(url)));
			}
		}
	});

}

concurrency::task<std::pair<account::postContentType, previewHelperbase*>> account::serviceHelpers::urlHelper(Windows::Foundation::Uri ^ url)
{
		auto str = url->ToString();
		auto it = domainMap.find(std::wstring_view(url->Domain->Data()));
		if (it != domainMap.end())
		{
			try {
				return it->second(url).then([](concurrency::task<std::pair<account::postContentType, previewHelperbase*>>t) {
					try { t.wait(); return t; }
					catch (Platform::Exception^ e)
					{
						__debugbreak();
					}
					catch (std::exception e)
					{
						__debugbreak();
					}
					catch (...)
					{
						__debugbreak();
					}
				});;
			}
			catch (...)
			{
				__debugbreak();
			}
		}
		else
		{
			return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>(account::postContentType::linktype, new previewHelperbase(url)));
		}

}

std::tuple<account::postContentType, previewHelperbase*, const ServiceHelper* const> account::serviceHelpers::prelimContentHelper(Windows::Data::Json::JsonObject ^ z)
{
	if (z->HasKey("crosspost_parent"))
	{
		return std::tuple<account::postContentType, previewHelperbase*, const ServiceHelper* const>(postContentType::xpost, new CrossPostDisplay(ref new account::subpostUWP(z->GetNamedArray("crosspost_parent_list")->GetObjectAt(0))), nullptr);
	}
	auto it = domainMap.find(std::wstring_view(z->GetNamedString("domain")->Data()));
	if (it != domainMap.end())
	{
		auto r = it->second.prelimCall(z);
		return std::tuple<account::postContentType, previewHelperbase*, const ServiceHelper* const>{std::move(r.first), std::move(r.second), &it->second };
	}
	else
	{
		return { account::postContentType::linktype, new previewHelperbase(ref new Windows::Foundation::Uri(z->GetNamedString("url"))), nullptr };
	}
}
std::tuple<account::postContentType, previewHelperbase*, const ServiceHelper* const> account::serviceHelpers::prelimContentHelper(Windows::Foundation::Uri ^ url)
{
	auto it = domainMap.find(std::wstring_view(url->Domain->Data()));
	if (it != domainMap.end())
	{
		auto r = it->second.prelimCall(url);
		return std::tuple<account::postContentType, previewHelperbase*, const ServiceHelper* const>{std::move(r.first), std::move(r.second), &it->second };
	}
	else
	{
		return { account::postContentType::linktype, nullptr, nullptr };
	}
}
const struct imgur final : ServiceHelper
{
	imgur() {}
	urioperator
	{
		try
		{
		bool testimagehint = true;
		std::wstring id = uri->Path->Data();
		const std::wregex remove{ L"[^/]*$" };
		if (id.compare(0, 3, L"/a/") == 0)
		{
			id = id.substr(3);
		}
		else
		{
			std::wsmatch result;
			std::regex_search(id, result, remove);
			id = result.str();
		}
		size_t lastindex = id.find_last_of(L"."); //get rid of extension
		if (lastindex != std::wstring::npos)
			id = id.substr(0, lastindex);
		Platform::String^ idRefStr = ref new Platform::String(id.data());
		auto imageEndpoint = [uri, idRefStr]()
		{
			auto u = uri->ToString();
			OutputDebugString(u->Data());
			auto url = ref new Windows::Foundation::Uri("https://api.imgur.com/3/image/", idRefStr);
			return concurrency::create_task(globalvars::imgurHttp->GetAsync(url)).then([uri](Windows::Web::Http::HttpResponseMessage^ z) {
				z->EnsureSuccessStatusCode();
				auto json = Windows::Data::Json::JsonObject::Parse(z->Content->ToString())->GetNamedObject("data");
				if (json->GetNamedBoolean("animated"))
				{
					return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::giftype, new gifvDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("mp4")), uri));
				}
				else
				{
					return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, new imageDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("link")), uri));
				}

			});
		};
		auto albumEndpoint = [uri, idRefStr]()
		{

			return concurrency::create_task(globalvars::imgurHttp->GetAsync(ref new Windows::Foundation::Uri("https://api.imgur.com/3/album/",idRefStr))).then([uri](Windows::Web::Http::HttpResponseMessage^ z) {

				try
				{
					z->EnsureSuccessStatusCode();
					auto json = Windows::Data::Json::JsonObject::Parse(z->Content->ToString())->GetNamedObject("data");
					auto arrazy = json->GetNamedArray("images");
					Platform::Collections::Vector<Windows::Foundation::Uri^>^ vec = ref new Platform::Collections::Vector<Windows::Foundation::Uri^>(arrazy->Size);
					std::vector<std::unique_ptr<account::serviceHelpers::previewHelperbase>> Items;
					for (auto &&a : arrazy)
					{
						const auto &b = a->GetObject();
						if (b->GetNamedBoolean("animated"))
						{
							try {
								Items.emplace_back(std::make_unique<gifvDisplay>(ref new Windows::Foundation::Uri(b->GetNamedString("mp4")), uri));
							}
							catch (...)
							{
								goto imageAlbumitem;
							}
						}
						else
						{
							imageAlbumitem:
							Items.emplace_back(std::make_unique<imageDisplay>(ref new Windows::Foundation::Uri(b->GetNamedString("link")), uri));
						}
					}
					/*std::transform(Windows::Foundation::Collections::begin(arrazy), Windows::Foundation::Collections::end(arrazy), Windows::Foundation::Collections::begin(vec), [](Windows::Data::Json::IJsonValue^ vz) {
					return ref new Windows::Foundation::Uri(static_cast<Windows::Data::Json::JsonObject^>(vz)->GetNamedString("link"));
					});*/
					return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::albumtype, new albumDisplay(uri, std::move(Items)));
				}
				catch (...)
				{
					__debugbreak();
				}
			});
		};

		//query the image endpoint first if we suspect that it is the right one
		if (hint != account::postContentType::albumtype)
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
		}).then([uri](concurrency::task<std::pair<account::postContentType, previewHelperbase*>> albumTask) {
			try
			{
				albumTask.wait();
				return albumTask;
			}
			catch (...)
			{
				return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>{account::postContentType::linktype, new previewHelperbase(uri)});
			}
		});
		else
		{
			return albumEndpoint().then([imageEndpoint](concurrency::task<std::pair<account::postContentType, previewHelperbase*>> imageTask) {
				try
				{
					imageTask.wait();
					return imageTask;
				}
				catch (...)
				{
					return imageEndpoint();
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
	}
	catch (...)
	{
		return concurrency::task_from_result(std::pair<account::postContentType, previewHelperbase*>{account::postContentType::linktype, new previewHelperbase(uri)});
	}
	}
	jsonoperator
	{
		return concurrency::create_task([j = std::move(json), h= std::move(hint), this]() {
		account::postContentType hint = h;
		if (h == account::postContentType::selftype)
		{
			auto prelim = prelimCall(j);
			if (prelim.second != nullptr)
			{
				return concurrency::task_from_result(std::move(prelim));
			}
			else
			{
				hint = prelim.first;
			}
		}
		return this->operator()(ref new Windows::Foundation::Uri(j->GetNamedString("url")), hint);
	});
	}
	puricall
	{
		if (uri->Extension == L"jpg" || uri->Extension == L"png")
		{
			return { account::postContentType::imagetype, nullptr };
		}
		else if (uri->Extension == L"gifv" || uri->Extension == L"mp4" || uri->Extension == L"gif" || uri->Extension == L"webm")
		{
			return { account::postContentType::giftype, nullptr };
		}
		std::wstring_view v(uri->ToString()->Data());
		const auto it = v.find(L"/a/", 8);
		if (it != std::wstring_view::npos)
		{
			return { account::postContentType::albumtype, nullptr };
		}
		else
		{
			const auto i = v.find(L"/g/", 8);
			if (i != std::wstring_view::npos)
			{
				return { account::postContentType::albumtype, nullptr };
			}
			else
			{
				return { account::postContentType::imagetype, nullptr };
			}
		}
	}
	pjsoncall
	{
		auto uri = ref new Windows::Foundation::Uri(json->GetNamedString("url"));
		try 
		{
			if (json->GetNamedString("post_hint") == L"image")
			{
				return { account::postContentType::imagetype, new imageDisplay(std::move(uri)) };
			}
		}
		catch (...) {}
		try
		{
			const auto& prev = json->GetNamedObject("preview");
			if (prev->HasKey("reddit_video_preview"))
			{
				if ((json->GetNamedString("domain") == L"i.imgur.com"))
				{
					if (uri->Extension == L"mp4")
					{
						return { account::postContentType::giftype, new gifvDisplay(uri, uri) };
					}
					else if (uri->Extension == L"gifv" || uri->Extension == L"gif")
					{
						std::wstring str = uri->ToString()->Data();
						const auto i = str.rfind(L'.', str.length());
						if (i != std::wstring::npos)
						{
							str.replace(i + 1, 3, L"mp4");
							return { account::postContentType::giftype, new gifvDisplay(std::move(uri), ref new Windows::Foundation::Uri(Platform::StringReference(str.data()))) };
						}
						else
						{
							return { account::postContentType::giftype, nullptr };
						}
					}
				}
				else
				{
					return { account::postContentType::giftype, nullptr };
				}
			}
		}
		catch (...)
		{

		}
		try {
			if (json->HasKey("oembed"))
			{
				const auto& imgurtype = json->GetNamedObject("oembed")->GetNamedString("type");
				if (imgurtype == "video")
				{
					return { account::postContentType::giftype, nullptr };
				}
				else if(imgurtype == "rich")
				{
					return { account::postContentType::albumtype, nullptr };
				}
			}
		}
		catch (...) {}
		return prelimCall(std::move(uri));
	}
}imgurHelper;
const struct iReddit final : ServiceHelper
{
	iReddit() {}
	urioperator
	{
		auto http = ref new HttpRequestMessage(Windows::Web::Http::HttpMethod::Head, uri);
		auto xx = new imageDisplay(uri, uri);
		return concurrency::create_task(globalvars::generalHttp->SendRequestAsync(http, Windows::Web::Http::HttpCompletionOption::ResponseHeadersRead)).then([xx](Windows::Web::Http::HttpResponseMessage^ m) {
			if (m->Content->Headers->ContentType->MediaType == "image/gif")
			{
				return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::giftype, xx );
			}
			else
			{
				std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, xx);
			}
		});
	}
	puricall
	{
		if (uri->Extension == "gif")
		{
			return { account::postContentType::giftype, new imageDisplay(uri, uri) };
		}
		else
		{
			return { account::postContentType::imagetype, new imageDisplay(uri, uri) };
		}
	}
	pjsoncall
	{
		auto url = ref new Windows::Foundation::Uri(json->GetNamedString("url"));
		try 
		{
			const auto& p = json->GetNamedObject("preview");
			try
			{
				if (p->HasKey("variants"))
				{
					const auto & v = p->GetNamedObject("variants");
					if (v->HasKey("mp4"))
					{
						return { account::postContentType::giftype, new gifvDisplay(ref new Windows::Foundation::Uri(v->GetNamedObject("source")->GetNamedString("url")), url) };
					}
					else if (url->Extension == "gif")
					{
						return { account::postContentType::giftype, new imageDisplay(url, url) };
					}
					else
					{
						return { account::postContentType::imagetype, new imageDisplay(url, url) };
					}
				}
				else
				{
					if (url->Extension == "gif")
					{
						return { account::postContentType::giftype, new imageDisplay(url, url) };
					}
					else
					{
						return { account::postContentType::imagetype, new imageDisplay(url, url) };
					}
				}
			}
			catch (...)
			{
				if (url->Extension == "gif")
				{
					return { account::postContentType::giftype, new imageDisplay(url, url) };
				}
				else
				{
					return { account::postContentType::imagetype, new imageDisplay(url, url) };
				}
			}

		}
		catch (...)
		{
			if (url->Extension == "gif")
			{
				return { account::postContentType::giftype, new imageDisplay(url, url) };
			}
			else
			{
				return { account::postContentType::imagetype, new imageDisplay(url, url) };
			}
		}
	}
}iRedditHelper;
const struct gfycat final : ServiceHelper
{
	gfycat() {}
	urioperator
	{
		auto requestUri = ref new Windows::Foundation::Uri(L"https://gfycat.com/cajax/get" + uri->Path);
		return concurrency::create_task(globalvars::generalHttp->GetAsync(requestUri)).then([uri](Windows::Web::Http::HttpResponseMessage^ s) {
		try {
			s->EnsureSuccessStatusCode();
			auto js = Windows::Data::Json::JsonObject::Parse(s->Content->ToString());
			const auto &xx = js->GetNamedObject("gfyItem");
			return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::giftype, new QualityScaleGifv({
				{
					L"Max Quality", ref new Windows::Foundation::Uri(xx->GetNamedString("mp4Url")) 
				},
				{
					L"Mobile", ref new Windows::Foundation::Uri(xx->GetNamedString("mobileUrl"))
				},
				{
					L"Mini", ref new Windows::Foundation::Uri(xx->GetNamedString("miniUrl"))
				}
				}, uri));
		}
		catch (Platform::Exception^ e)
		{
			__debugbreak();
		}

	});
	}
	puricall
	{
		const auto &path = uri->Path;
		std::vector < std::pair<const std::wstring, Windows::Foundation::Uri^> > q;
		q.push_back({L"Max Quality", ref new Windows::Foundation::Uri("https://giant.gfycat.com/" + path + ".mp4")});
		q.push_back({L"Mobile", ref new Windows::Foundation::Uri("https://thumbs.gfycat.com/" + path + "-mobile.mp4")});
		q.push_back({ L"Mini", ref new Windows::Foundation::Uri("https://thumbs.gfycat.com/" + path + "-mini.mp4") });
		return { account::postContentType::giftype, new QualityScaleGifv(std::move(q), uri) };
	}
	pjsoncall
	{
		try {
			if ((json->HasKey("post_hint") && (json->GetNamedString("post_hint" ) == "rich:video") ) || (json->GetNamedObject("secure_media")->GetNamedObject("oembed")->GetNamedString("type") == "video"))
		{
			return prelimCall(ref new Windows::Foundation::Uri(json->GetNamedString("url")));
		}
		}
		catch (...)
		{

		}
			return {postContentType::giftype, new previewHelperbase(ref new Windows::Foundation::Uri(json->GetNamedString("url")))};
	}
}gfycatHelper;
const struct streamable final : ServiceHelper
{
	streamable() {}
	urioperator
	{
		auto reqURI = ref new Windows::Foundation::Uri(L"https://api.streamable.com/videos" + uri->Path);
		return concurrency::create_task(globalvars::generalHttp->GetAsync(reqURI)).then([uri](Windows::Web::Http::HttpResponseMessage^ x) {
		try {
			x->EnsureSuccessStatusCode();

			auto directURI = ref new Windows::Foundation::Uri("http://", JsonObject::Parse(x->Content->ToString())->GetNamedObject("files")->GetNamedObject("mp4")->GetNamedString("url"));
			return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::videotype, new videoDisplay(directURI, uri));
		}
		catch (...)
		{
			__debugbreak();
		}
	});
	}
	jsonoperator
	{
		auto x = prelimCall(json);
		if (x.first == postContentType::videotype)
		{
			return operator()(ref new Windows::Foundation::Uri(json->GetNamedString("url")));
		}
	}
	puricall
	{
		return {postContentType::videotype, nullptr};
	}
	pjsoncall
	{
		if (json->GetNamedString("post_hint") == "rich:video")
		{
			return prelimCall(ref new Windows::Foundation::Uri(json->GetNamedString("url")));
		}
		else
		{
			return { postContentType::linktype, new previewHelperbase(ref new Windows::Foundation::Uri(json->GetNamedString("url"))) };
		}
	}
}streamableHelper;
const struct vReddit final : ServiceHelper
{
	vReddit() {}
	jsonoperator
	{
		auto x = prelimCall(json);
	if (x.second != nullptr)
	{
		return concurrency::task_from_result(std::move(x));
		}
	else
	{
		return operator()(ref new Windows::Foundation::Uri(json->GetNamedString("url")));
	}
	}
		urioperator
	{
	return concurrency::create_task(globalvars::currentacc->GetRedirectUri(uri)).then([](Windows::Foundation::Uri^ u) {return globalvars::currentacc->httpClient->GetStringAsync(ref new Windows::Foundation::Uri(u->ToString() + ".json")); }).then([uri](Platform::String^ s) {
		try
		{
			return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::xpost, new CrossPostDisplay(ref new account::subpostUWP(Windows::Data::Json::JsonArray::Parse(s)->GetObjectAt(0)->GetNamedObject("data")->GetNamedArray("children")->GetObjectAt(0)->GetNamedObject("data"))));
		}
		catch (...)
		{
			try
			{
				auto hlsurl = uri->ToString();
				hlsurl += L"/HLSPlaylist.m3u8";
				auto hlsuri = ref new Windows::Foundation::Uri(std::move(hlsurl));
				auto http = ref new HttpRequestMessage(Windows::Web::Http::HttpMethod::Head, hlsuri);
				return concurrency::create_task(globalvars::generalHttp->SendRequestAsync(http, Windows::Web::Http::HttpCompletionOption::ResponseHeadersRead)).then([hlsuri, uri](Windows::Web::Http::HttpResponseMessage^ m) {
					if (m->Content->Headers->ContentType->MediaType == "application/x-mpegURL")
					{
						return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::videotype, new videoDisplay(hlsuri, uri));
					}
					else
					{
						std::pair<account::postContentType, previewHelperbase*>(account::postContentType::linktype, new previewHelperbase(uri));
					}
				}).get();
			}
			catch (...)
			{
				return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::linktype, new previewHelperbase(uri));
			}
		}
	});
		}
	puricall
	{
		auto hlsurl = uri->ToString();
		hlsurl += L"/HLSPlaylist.m3u8";
		return {postContentType::videotype, new videoDisplay(ref new Windows::Foundation::Uri(std::move(hlsurl)), uri)};
	}
	pjsoncall
	{
		try {
		auto rd = json->GetNamedObject("secure_media")->GetNamedObject("reddit_video");
		auto videoUrl = ref new Windows::Foundation::Uri(rd->GetNamedString("hls_url"));
		auto url = ref new Windows::Foundation::Uri(json->GetNamedString("url"));
		if (rd->GetNamedBoolean("is_gif"))
		{
			return { postContentType::giftype, new gifvDisplay(std::move(videoUrl),std::move(url)) };
		}
		else
		{
			return { postContentType::videotype, new videoDisplay(std::move(videoUrl), std::move(url)) };
		}
		}
	catch (...)
	{
		return {postContentType::videotype, nullptr};
 }
		
	}
}vRedditHelper;

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

account::serviceHelpers::gifvDisplay::gifvDisplay(Windows::Foundation::Uri ^ imageDirectLink, Windows::Foundation::Uri ^ Url) : imageDisplay(imageDirectLink, Url)
{
		source = Windows::Media::Core::MediaSource::CreateFromUri(contentLink);
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::gifvDisplay::viewerControl()
{
	try {
		auto x = this->contentLink->ToString();
		auto v = this->Link->ToString();
		auto element = ref new Windows::UI::Xaml::Controls::MediaPlayerElement();
		element->Source = this->source;
		element->MediaPlayer->IsLoopingEnabled = true;
		element->AutoPlay = true;
		element->AreTransportControlsEnabled = false;
		element->Stretch = Windows::UI::Xaml::Media::Stretch::Uniform;
		return element;
	}
	catch (...)
	{
		
		__debugbreak();
	}
	
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
	if (ControlsList == nullptr)
	{
		ControlsList = ref new Platform::Collections::Vector<Windows::UI::Xaml::UIElement^>(items.size());
		std::transform(items.cbegin(), items.cend(), Windows::Foundation::Collections::begin(ControlsList), [](auto &a) {
			return a->viewerControl();
		});
	}
	flipview->ItemsSource = ControlsList;
	return flipview;
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::embedDisplay::viewerControl()
{
	auto webview = ref new Windows::UI::Xaml::Controls::WebView(Windows::UI::Xaml::Controls::WebViewExecutionMode::SeparateThread);
	webview->NavigateToString(framestr);
	return webview;
}

account::serviceHelpers::videoDisplay::videoDisplay(Windows::Foundation::Uri ^ imageDirectLink, Windows::Foundation::Uri ^ Url) : gifvDisplay(imageDirectLink, Url) , adaptive(false)
{
}

account::serviceHelpers::videoDisplay::videoDisplay(Windows::Foundation::Uri ^ imageDirectLink, Windows::Foundation::Uri ^ Url, Windows::Media::Streaming::Adaptive::AdaptiveMediaSource ^ source) : gifvDisplay(imageDirectLink, Url), adaptive(true)
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
//}const
const std::unordered_map<
	std::wstring_view,
	const ServiceHelper&
> account::serviceHelpers::domainMap = { { L"imgur.com" , imgurHelper },{ L"i.imgur.com", imgurHelper } ,{ L"i.redd.it", iRedditHelper },{ L"gfycat.com", gfycatHelper },{ L"streamable.com" , streamableHelper }, {L"v.redd.it", vRedditHelper} };

account::serviceHelpers::QualityScaleGifv::QualityScaleGifv(std::vector<std::pair<const std::wstring, Windows::Foundation::Uri^>> q, Windows::Foundation::Uri ^ url)
{
	Link = std::move(url);
	contentLink = q.begin()->second;
	Qualities = std::move(q);
	source = Windows::Media::Core::MediaSource::CreateFromUri(contentLink);
}

Windows::UI::Xaml::UIElement ^ account::serviceHelpers::CrossPostDisplay::viewerControl()
{
	auto contentPres = ref new Windows::UI::Xaml::Controls::ContentPresenter();
	contentPres->ContentTemplate = static_cast<Reddit::App^>(Windows::UI::Xaml::Application::Current)->listTemplateRes->listSelfPost;
	contentPres->Content = ParentPost.Resolve<account::subpostUWP>();
	return std::move(contentPres);
}
