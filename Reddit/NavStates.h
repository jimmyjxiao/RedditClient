#pragma once
#include "AccountInterface.h"
#include "subpostUWP.h"
#include "App.xaml.h"
#include "NavStates.h"
struct baseNavState;
namespace Reddit
{
	public ref class NavPage : Windows::UI::Xaml::Controls::Page
	{
	private:
		int navIndex;
	protected:
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override final;
		virtual void OnNavigatedToPageCode() {}
	protected private:

		baseNavState * baseRef;
	internal:
		virtual property Windows::UI::Xaml::Interop::TypeName PageType
		{
			virtual Windows::UI::Xaml::Interop::TypeName get()
			{
				return this->GetType();
			}
		}
		property int NavigationIndex 
		{
			int get()
			{
				return navIndex;
			}
		}
	};

}
struct baseNavState
{
	Reddit::NavPage^ pageState = nullptr;
};
struct subredditNavstate final : baseNavState
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
struct UserNavState final : baseNavState
{
	account::postSort sort = account::postSort::Defaultsort;
	account::timerange rng = account::timerange::Default;
	account::AccountInfo account;
};
struct commentNavstate final : baseNavState
{
	account::subpostUWP^ post;
	Platform::String^ postID;
	Platform::String^ before;
	Platform::String^ after;
	account::commentSort sort;
};
typedef std::pair<Windows::UI::Xaml::Interop::TypeName,baseNavState*> navVariant;