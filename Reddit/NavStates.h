#pragma once
#include "AccountInterface.h"
#include "subpostUWP.h"
#include "App.xaml.h"
enum class NavTypes : unsigned char
{
	subreddit, comment
};
struct baseNavState
{
	Reddit::NavIndexed^ pageState = nullptr;
};
struct subredditNavstate : baseNavState
{
	account::subredditInfo info;
	account::postSort sort = account::postSort::hot;
	account::timerange rng = account::timerange::Default;
	subredditNavstate(Platform::String^ s) {
		info.name = std::move(s);
		info.subredditIndex = INT_MAX;
	}
	subredditNavstate(Platform::String^ s, account::postSort ss, account::timerange t) : sort(std::move(ss)), rng(std::move(t)) {
		info.name = std::move(s);
		info.subredditIndex = INT_MAX;
	}
	subredditNavstate(account::subredditInfo i) : info(std::move(i)){}
};
struct commentNavstate : baseNavState
{
	account::subpostUWP^ post;
	Platform::String^ postID;
	Platform::String^ before;
	Platform::String^ after;
	account::commentSort sort;
};
union Navptrs
{
	subredditNavstate* s;
	commentNavstate* c;
	Navptrs(subredditNavstate* x) :s(x) {}
	Navptrs(commentNavstate* x) :c(x) {}
};
typedef std::pair<NavTypes,Navptrs> navVariant;