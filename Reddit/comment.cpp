#include "pch.h"
#include "comment.h"
#include "globalvars.h"


namespace account
{
	account::comment::comment(Windows::Data::Json::JsonObject ^ json, Platform::String^ plink, Platform::String^ subreddit) : VotableThing(json)
	{
		try {
			if (subreddit != nullptr)
			{
				JsonGetString(parent_subreddit, "subreddit", json);
			}
			else
			{
				parent_subreddit = std::move(subreddit);
			}
			try
			{

				controversial = (bool)json->GetNamedNumber("controversiality", 0);
			}
			catch (Platform::COMException^ e)
			{

			}
			JsonGetString(markdown, "body", json);
			if (plink == nullptr)
			{
				//JsonGetString(plink_id, "link_id", json);
				try {
					plink_id = json->GetNamedString("link_id");
				}
				catch (Platform::Exception^ e)
				{
					__debugbreak();
				}
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