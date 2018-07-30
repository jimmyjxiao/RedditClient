#pragma once
#include "VotableThing.h"
#include "accountEnums.h"

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
		imgurpending,
		xpost,
		none,
		pending

	};
	public enum class subType
	{
		link, self, all
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
		subType submissions;
		int64 subscribed;
		unsigned int subredditIndex;
	};
	struct subInfoCompare : std::equal_to<subredditInfo>
	{
		bool operator()(const subredditInfo& a, const subredditInfo&b)
		{
			return ((a.subscribers == b.subscribers) && (a.name == b.name) && (a.sidebar_html == b.sidebar_html) && (a.desc == b.desc) && (a.NSFW == b.NSFW) && (a.submissions == b.submissions) && (a.subscribed == b.subscribed));
		}
	};
	struct subpost : VotableThing
	{
		static subredditInfo getSubredditInfoFromJson(Windows::Data::Json::JsonObject^ sub);
		postContentType contentType;
		account::commentSort suggestedsort;
		subpost(Windows::Data::Json::JsonObject^ json);
		~subpost();
		Platform::String^ selftext;
		Platform::String^ Title;
		Windows::Foundation::Uri^ link;
		bool self = false;
		Platform::String^ subreddit;
		Platform::String^ subreddit_id;
		int num_comments;
		bool NSFW;
	};

}