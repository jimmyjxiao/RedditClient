#include "pch.h"
#include "serviceHelpers.h"
#include "globalvars.h"
#include <regex>
using namespace account;
using namespace account::serviceHelpers;
concurrency::task<std::pair<account::postContentType, previewHelperbase*>> serviceHelpers::urlHelper(Windows::Data::Json::JsonObject^ z)
{
	return concurrency::create_task([z]() {
		try
		{
			auto domain = z->GetNamedString("domain");
			auto url = ref new Windows::Foundation::Uri(z->GetNamedString("url"));
			auto excepted = [url]() {
				return std::pair<account::postContentType, previewHelperbase*>(postContentType::linktype, new previewHelperbase(url));
			};

			//imgur helper
			if (domain == L"i.redd.it")
			{
				account::postContentType type;
				if (url->Extension == "gif")
				{
					type = account::postContentType::giftype;
				}
				else
					type = account::postContentType::imagetype;
				return std::pair<account::postContentType, previewHelperbase*>(type, new imageDisplay(url));
			}
			else if (domain == L"imgur.com" || domain == "i.imgur.com")
			{
				try
				{
					if (z->GetNamedString("post_hint") == L"image")
					{
						return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, new imageDisplay(url));
					}
					else
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
								try
								{
									auto json = Windows::Data::Json::JsonObject::Parse(z->Content->ToString())->GetNamedObject("data");
									if (json->GetNamedBoolean("animated"))
									{
										return std::pair<account::postContentType, previewHelperbase*> (account::postContentType::giftype, new gifvDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("link")), url));
									}
									else
									{
										return std::pair<account::postContentType, previewHelperbase*>(account::postContentType::imagetype, new imageDisplay(ref new Windows::Foundation::Uri(json->GetNamedString("link")), url));
									}
								}
								catch (...)
								{
									__debugbreak();
								}
							});
						};
						auto albumEndpoint = [url, id]()
						{
							return concurrency::create_task(globalvars::imgurHttp->GetAsync(ref new Windows::Foundation::Uri("https://api.imgur.com/3/album/", Platform::StringReference(id.data())))).then([url](Windows::Web::Http::HttpResponseMessage^ z) {
								z->EnsureSuccessStatusCode();
								try
								{
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
								catch(...)
								{__debugbreak(); }
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
						}).get();
					}
				}
				catch (...)
				{
					__debugbreak();
					return excepted();
				}
			}
			else
			{
				return excepted();
			}
		}
		catch (Platform::Exception^ e)
		{
			__debugbreak();
		}
	});

}
template<size_t size>
std::array<previewHelperbase, size> serviceHelpers::getManyHelpers(std::vector<Windows::Data::Json::JsonObject^>const & vec)
{
	//std::vector<Windows::Data::Json::JsonObject^> imgurVec(), gfycatVec(), httpHeadVec();
	throw ref new Platform::NotImplementedException();
}

