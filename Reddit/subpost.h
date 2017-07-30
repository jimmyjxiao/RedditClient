#pragma once
#include "VotableThing.h"
namespace account
{
	public enum class postContentType
	{
		selftype = 0,
		imagetype = 1,
		linktype = 2,
		giftype = 3,
		videotype = 4,
		adtype = 5,
		ytvideotype = 6,
		albumtype = 7,
		imgurpending

	};
	public value struct subredditInfo
	{
		Platform::String^ name;
		Platform::String^ pname;
		Platform::String^ sidebar_html;
		Platform::String^ desc;
		Windows::UI::Color key_color;
		bool NSFW;
		unsigned int subscribers;
	};
	struct subpost : VotableThing, RedditCreated
	{
		static subredditInfo getSubredditInfoFromJson(Windows::Data::Json::JsonObject^ sub);
		postContentType contentType;
		subpost(Windows::Data::Json::JsonObject^ json);
		~subpost();
		Platform::String^ selftext_html;
		Platform::String^ Title;
		Platform::String^ link;
		bool self = false;
		Platform::String^ subreddit;
		Platform::String^ subreddit_id;
		int num_comments;
		bool NSFW;
	};

}