#pragma once
#include "AccountInterface.h"
#include "subpostUWP.h"
#include "App.xaml.h"
namespace Reddit
{
	public interface struct NavIndexed
	{
		property int NavigationIndex
		{
			int get();
		}
		property Windows::UI::Xaml::Interop::TypeName PageType
		{
			Windows::UI::Xaml::Interop::TypeName get();
		}
	};
}
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
struct UserNavState : baseNavState
{
	account::postSort sort = account::postSort::Defaultsort;
	account::timerange rng = account::timerange::Default;
	account::AccountInfo account;
};
struct commentNavstate : baseNavState
{
	account::subpostUWP^ post;
	Platform::String^ postID;
	Platform::String^ before;
	Platform::String^ after;
	account::commentSort sort;
};
typedef std::pair<Windows::UI::Xaml::Interop::TypeName,baseNavState*> navVariant;