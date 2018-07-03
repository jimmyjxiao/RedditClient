#include "pch.h"
#include "subpost.h"
#include <string>
#include "AccountInterface.h"
namespace account
{
	
	subredditInfo subpost::getSubredditInfoFromJson(Windows::Data::Json::JsonObject ^ sub)
	{
		try
		{
			subredditInfo info;
			info.name = sub->GetNamedString("display_name");
			std::wstring colorstr = sub->GetNamedString("key_color")->Data();
			if (colorstr == L"")
			{
				info.key_color = Windows::UI::Colors::Transparent;
			}
			else
			{
				int hexValue = std::stol(colorstr.substr(1), nullptr, 16);
				info.key_color = Windows::UI::ColorHelper::FromArgb(255, ((hexValue >> 16) & 0xFF), ((hexValue >> 8) & 0xFF), ((hexValue) & 0xFF));
			}
			info.sidebar_html = sub->GetNamedString("description_html");
			info.desc = sub->GetNamedString("public_description");
			info.subscribers = sub->GetNamedNumber("subscribers");
			info.pname = sub->GetNamedString("display_name_prefixed");
			info.NSFW = sub->GetNamedBoolean("over18");
			auto typestr = sub->GetNamedString("submission_type");
			if (typestr == "self")
			{
				info.submissions = subType::self;
			}
			else if (typestr == "any")
			{
				info.submissions = subType::all;
			}
			else if (typestr == "link")
			{
				info.submissions = subType::link;
			}
			info.subredditIndex = -1;
			return info;
		}
		catch (...)
		{
			__debugbreak();
		}
	}
	subpost::subpost(Windows::Data::Json::JsonObject^ json) : VotableThing(json), RedditCreated(json)
	{
		try
		{
			subreddit = json->GetNamedString("subreddit");
			if (json->GetNamedBoolean("is_self"))
			{
				self = true;
				try
				{
					selftext = json->GetNamedString("selftext");
				}
				catch (...)
				{
					selftext = nullptr;
				}
			}
			else
			{
				link = ref new Windows::Foundation::Uri(json->GetNamedString("url"));
			}
			auto sug = json->GetNamedValue("suggested_sort");
			if (sug->ValueType == Windows::Data::Json::JsonValueType::Null)
			{
				suggestedsort = commentSort::default;
			}
			else
			{
				auto sugstr = sug->GetString();
				if (sugstr == "top")
				{
					suggestedsort == commentSort::top;
				}
				else if (sugstr == "new")
				{
					suggestedsort == commentSort::New;
				}
				else if (sugstr == "random")
				{
					suggestedsort == commentSort::random;
				}
				else if (sugstr == "qa")
				{
					suggestedsort == commentSort::qa;
				}
				else if (sugstr == "old")
				{
					suggestedsort == commentSort::old;
				}
				else if (sugstr == "confidence")
				{
					suggestedsort == commentSort::best;
				}
				else if (sugstr == "controversial")
				{
					suggestedsort = commentSort::controversial;
				}
			}
			NSFW = json->GetNamedBoolean("over_18");
			Title = json->GetNamedString("title");
			num_comments = json->GetNamedNumber("num_comments");
			if (self)
				contentType = postContentType::selftype;
			/*else if (json->GetNamedValue("media")->ValueType != Windows::Data::Json::JsonValueType::Null)
			{
				try {
					auto obj = json->GetNamedObject("media");
					if (obj->HasKey("oembed"))
					{
						auto xembed = obj->GetNamedObject("oembed");
						Platform::String^ provider = xembed->GetNamedString("provider_name");
						if (provider == L"Imgur")
						{

							Platform::String^ imgurtype = xembed->GetNamedString("type");
							if (imgurtype == L"video")
								contentType = postContentType::giftype;
							else if (imgurtype == L"rich")
							{
								contentType = postContentType::albumtype;
							}
							else
								contentType = postContentType::imgurpending;

						}
						else if (provider == L"gfycat")
							contentType = postContentType::giftype;
						else if (provider == L"YouTube")
						{
							contentType = postContentType::ytvideotype;
						}
						else
							contentType = postContentType::linktype;
					}
					else if (obj->HasKey("reddit_video"))
					{
						auto rvid = obj->GetNamedObject("reddit_video");
						if (rvid->GetNamedBoolean("is_gif"))
							contentType = postContentType::giftype;
						else
							contentType = postContentType::videotype;
					}
				}
				catch (...)
				{
					contentType = postContentType::linktype;
				}
			}*/
		}
		catch (...)
		{
			__debugbreak();
		}
	}


	subpost::~subpost()
	{
		

	}
}