#include "pch.h"
#include "subpost.h"
#include <string>
namespace account
{
	
	subredditInfo subpost::getSubredditInfoFromJson(Windows::Data::Json::JsonObject ^ sub)
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
		info.subscribers = sub->GetNamedNumber("subscribers");
		info.pname = sub->GetNamedString("display_name_prefixed");
		info.NSFW = sub->GetNamedBoolean("over18");
		return info;
	}
	subpost::subpost(Windows::Data::Json::JsonObject^ json) : VotableThing(json), RedditCreated(json)
	{
		subreddit = json->GetNamedString("subreddit");
		if (json->GetNamedBoolean("is_self"))
		{
			self = true;
			try
			{
				selftext_html = json->GetNamedString("selftext_html");
			}
			catch (...)
			{
				selftext_html = "";
			}
		}
		NSFW = json->GetNamedBoolean("over_18");
		Title = json->GetNamedString("title");
		num_comments = json->GetNamedNumber("num_comments");
		if (self)
			contentType = postContentType::selftype;
		else if (json->GetNamedValue("media")->ValueType != Windows::Data::Json::JsonValueType::Null)
		{

			auto xembed = json->GetNamedObject("media")->GetNamedObject("oembed");
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
		else
		{
			contentType = postContentType::linktype;
		}
	}


	subpost::~subpost()
	{
	}
}