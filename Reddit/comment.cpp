#include "pch.h"
#include "comment.h"



namespace account
{
	account::comment::comment(Windows::Data::Json::JsonObject ^ json) : VotableThing(json), RedditCreated(json)
	{
		htmltext = json->GetNamedString("body_html");
		controversial = (bool)json->GetNamedNumber("controversiality");
	}
}