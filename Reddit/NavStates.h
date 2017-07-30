#pragma once
#include "AccountInterface.h"
#include "subpostUWP.h"
struct subredditNavstate
{
	Platform::String^ _subreddit = nullptr;
	account::postSort sort = account::postSort::hot;
	account::timerange rng = account::timerange::Default;
};
struct commentNavstate
{
	account::subpostUWP^ post;
	Platform::String^ postID;
	Platform::String^ before;
	Platform::String^ after;
	account::commentSort sort;
};