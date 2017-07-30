//
// SubRedditViewPage.xaml.cpp
// Implementation of the SubRedditViewPage class
//

#include "pch.h"
#include "SubRedditViewPage.xaml.h"
#include "globalvars.h"
#include "CommentViewPage.xaml.h"
using namespace Reddit;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

SubRedditViewPage::SubRedditViewPage()
{
	InitializeComponent();			
}


void Reddit::SubRedditViewPage::Range::set(account::timerange newrange)
{
	if (newrange != _rng && nav != nullptr)
	{

		posts = globalvars::currentacc->getsubredditAsyncVec(_subreddit, _sort, newrange)->listing;
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("posts"));
		_rng = newrange;
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Range"));
	}
}

void Reddit::SubRedditViewPage::Sort::set(account::postSort newsort)
{
	if (newsort != _sort && nav != nullptr)
	{
		timeSelector->SelectedIndex = 3;
		if (newsort == account::postSort::controversial || newsort == account::postSort::top)
		{
			_rng = account::timerange::month;
			posts = globalvars::currentacc->getsubredditAsyncVec(_subreddit, newsort, _rng)->listing;
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("posts"));
		}
		else
		{
			posts = globalvars::currentacc->getsubredditAsyncVec(_subreddit, newsort, account::timerange::Default)->listing;
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("posts"));
		}
		_sort = newsort;
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Sort"));
	}
}


void Reddit::SubRedditViewPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto _nav = static_cast<subredditNavstate*>(globalvars::NavState.find(static_cast<unsigned char>(e->Parameter))->second);
	_subreddit = _nav->_subreddit;
	_sort = _nav->sort;
	_rng = _nav->rng;
	if (_nav->_subreddit == nullptr)
	{
		sideBarButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		_subInfo.pname = L"Frontpage";
		posts = globalvars::currentacc->getsubredditAsyncVec()->listing;
		commandBar->Background = static_cast<Windows::UI::Xaml::Media::SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlBackgroundChromeMediumBrush"));
		subTextblock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	else
	{
		_subInfo.name = _nav->_subreddit;
		globalvars::currentacc->getJsonFromBasePath("r/" + _nav->_subreddit + "/about.json?raw_json=1").then([this, _nav](Windows::Data::Json::JsonObject^ response) {
			_subInfo = account::subpost::getSubredditInfoFromJson(response->GetNamedObject("data"));
			posts = globalvars::currentacc->getsubredditAsyncVec(_subreddit, _sort, _rng)->listing;
			this->sidebarWebview->NavigateToString(_subInfo.sidebar_html);
			int rngIndex;
			if (((int)_rng) == 6)
				rngIndex = 3;
			else
				rngIndex = (int)_rng;
			timeSelector->SelectedIndex = rngIndex;
			sortSelector->SelectedIndex = (int)_sort;
			Bindings->Update();
			nav = _nav;
		});
	}

}

void Reddit::SubRedditViewPage::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs ^ e)
{

}

void Reddit::SubRedditViewPage::sortSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	Sort = static_cast<account::postSort>(static_cast<ListView^>(sender)->SelectedValue);
}


void Reddit::SubRedditViewPage::rangeSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	Range = static_cast<account::timerange>(static_cast<ListView^>(sender)->SelectedValue);
}


void Reddit::SubRedditViewPage::listGrid_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto navstuff = new commentNavstate();
	auto clickedPost = static_cast<account::subpostUWP^>(e->ClickedItem);
	navstuff->postID = clickedPost->helper.id;
	navstuff->post = clickedPost;
	globalvars::NavState.insert(std::make_pair(globalvars::navChar, (void*)navstuff));
	globalvars::navChar++;
	((Windows::UI::Xaml::Controls::Frame^)Parent)->Navigate(CommentViewPage::typeid, unsigned char(globalvars::navChar - 1));
}


void Reddit::SubRedditViewPage::listGrid_ChoosingItemContainer(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ChoosingItemContainerEventArgs^ args)
{
	auto item = static_cast<account::subpostUWP^>(args->Item);
	if(!item->self)
	{
		if (args->ItemContainer != nullptr)
		{
			auto displaytype = static_cast<displayType>(static_cast<unsigned char>(args->ItemContainer->Tag));
			if (displaytype != displayType::content)
			{
				if (displaytype == displayType::self)
					SelfItemRecycle.push_back(args->ItemContainer);
				else if (displaytype == displayType::ad)
					AdItemRecycle.push_back(args->ItemContainer);
				goto retrievecontentTem;
			}
			else
			{
				auto it = std::find(ContentItemRecycle.begin(), ContentItemRecycle.end(), args->ItemContainer);
				if (it != ContentItemRecycle.end())
				{
					std::swap(*it, ContentItemRecycle.back());
					ContentItemRecycle.pop_back();
				}
			}
		}
		else
		{
		retrievecontentTem:
			if (ContentItemRecycle.size() > 0)
			{
				args->ItemContainer = ContentItemRecycle.back();
				ContentItemRecycle.pop_back();
			}
			else
			{
				auto newitem = ref new Windows::UI::Xaml::Controls::GridViewItem();
				newitem->ContentTemplate = imageGriddatatemplate;
				newitem->Tag = (unsigned char)displayType::content;
				args->ItemContainer = newitem;
			}
		}
	}
	else if (item->self)
	{
		if (args->ItemContainer != nullptr)
		{
			auto displaytype = static_cast<displayType>(static_cast<unsigned char>(args->ItemContainer->Tag));
			if (displaytype != displayType::self)
			{
				if (displaytype == displayType::content)
					ContentItemRecycle.push_back(args->ItemContainer);
				else if (displaytype == displayType::ad)
					AdItemRecycle.push_back(args->ItemContainer);
				goto retrieveselfTem;
			}
			else
			{
				auto it = std::find(SelfItemRecycle.begin(), SelfItemRecycle.end(), args->ItemContainer);
				if (it != SelfItemRecycle.end())
				{
					std::swap(*it, SelfItemRecycle.back());
					SelfItemRecycle.pop_back();
				}
			}
		}
		else
		{
		retrieveselfTem:
			if (SelfItemRecycle.size() > 0)
			{
				args->ItemContainer = SelfItemRecycle.back();
				SelfItemRecycle.pop_back();
			}
			else
			{
				auto newitem = ref new Windows::UI::Xaml::Controls::GridViewItem();
				newitem->ContentTemplate = selfGridTemplate;
				newitem->Tag = (unsigned char)displayType::self;
				args->ItemContainer = newitem;
			}
		}
	}
	args->IsContainerPrepared = true;
}
