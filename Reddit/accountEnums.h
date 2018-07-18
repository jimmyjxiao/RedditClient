#pragma once
namespace account
{
	public enum class RedditType
	{
		subpost, comment, subreddit, user, message
	};
	public enum class timerange
	{
		hour, day, week, month, year, all, Default
	};
	public enum class postSort
	{
		hot, New, rising, controversial, top, gilded, Defaultsort
	};
	public enum class commentSort
	{
		best, top, New, controversial, old, qa, random, default
	};
}