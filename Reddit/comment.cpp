#include "pch.h"
#include "comment.h"



namespace account
{
	account::comment::comment(Windows::Data::Json::JsonObject ^ json, Platform::String^ plink) : VotableThing(json), RedditCreated(json)
	{
		try {
			htmltext = json->GetNamedString("body_html");
			controversial = (bool)json->GetNamedNumber("controversiality");
			markdown = json->GetNamedString("body");
			if (plink == nullptr)
			{
				plink_id = json->GetNamedString("link_id");
			}
			else
			{
				plink_id = plink;
			}
		}
		catch (...)
		{
			__debugbreak();
		}
	}
	comment::~comment()
	{
		//__debugbreak();
	}
}