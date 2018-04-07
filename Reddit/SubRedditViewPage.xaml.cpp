//
// SubRedditViewPage.xaml.cpp
// Implementation of the SubRedditViewPage class
//

#include "pch.h"
#include "SubRedditViewPage.xaml.h"
#include "globalvars.h"
#include "CommentViewPage.xaml.h"
#include "ApplicationDataHelper.h"
#include "MyResources.xaml.h"
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


Reddit::SubRedditViewPage::~SubRedditViewPage()
{
	//__debugbreak();
}

SubRedditViewPage::SubRedditViewPage()
{
	Resources->Insert(ref new Platform::String(L"reportReasonsResource"),ref new Platform::String(L"dheh"));
	InitializeComponent();
}

bool SubRedditViewPage::SidebarUseCSS::get()
{
	return useCss;
}
void SubRedditViewPage::SidebarUseCSS::set(bool a)
{
	if (a != useCss)
	{
		useCss = a;
		updateSidebar();
	}
}
void Reddit::SubRedditViewPage::Range::set(account::timerange newrange)
{
	if (newrange != _rng && nav != nullptr)
	{

		_posts = globalvars::currentacc->getsubredditAsyncVec(_subreddit, _sort, newrange)->listing;
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
			lPtr = globalvars::currentacc->getsubredditAsyncVec(_subreddit, newsort, _rng);
			_posts = lPtr->listing;
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("posts"));
		}
		else
		{
			lPtr = globalvars::currentacc->getsubredditAsyncVec(_subreddit, newsort, account::timerange::Default);
			_posts = lPtr->listing;
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("posts"));
		}
		_sort = newsort;
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Sort"));
	}
}


void Reddit::SubRedditViewPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	navIndex = static_cast<unsigned char>(e->Parameter);
	auto _nav = globalvars::NavState.at(navIndex).second.s;
	_nav->pageState = this;
	_subreddit = _nav->info.name;
	_sort = _nav->sort;
	_rng = _nav->rng;
		if (_subreddit == nullptr)
		{
			sideBarButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			_subInfo.pname = L"Frontpage";
			lPtr = globalvars::currentacc->getsubredditAsyncVec();
			_posts = lPtr->listing;
			commandBar->Background = static_cast<Windows::UI::Xaml::Media::SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlBackgroundChromeMediumBrush"));
			subTextblock->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		else
		{
			lPtr = globalvars::currentacc->getsubredditAsyncVec(_subreddit, _sort, _rng);
			_posts = lPtr->listing;
			int rngIndex;
			if (((int)_rng) == 6)
				rngIndex = 3;
			else
				rngIndex = (int)_rng;
			timeSelector->SelectedIndex = rngIndex;
			sortSelector->SelectedIndex = (int)_sort;
			if (_nav->info.subredditIndex == INT_MAX)
			{
				_subInfo.name = _subreddit;
				auto stuff = [this, _nav](account::subredditInfo &z)
				{
					_subInfo = z;

					/*bool colorIsDark = (5 * _subInfo.key_color.G + 2 * _subInfo.key_color.R + _subInfo.key_color.B) <= 8 * 128;
					if (colorIsDark)
					{
						subredditBasedTheme = Windows::UI::Xaml::ElementTheme::Dark;
					}
					else
					{
						subredditBasedTheme = Windows::UI::Xaml::ElementTheme::Light;
					}*/
					this->updateSidebar();
					if (_subInfo.submissions == account::subType::self)
					{
						listingType = ViewMode::list;
					}
					else
					{
						listingType = ViewMode::grid;
					}
					nav = _nav;

				};
				try
				{
					auto x = ApplicationDataHelper::subredditHelpers::trysubredditInfoCache((const char16_t*)_subreddit->Data());
					Platform::Collections::Vector<account::reportReason>^ rules;
					std::wstring temp = x.sidebar_html->Data();
					temp.insert(27, L"usertext usertext-body md-container may-blank-within ");
					temp = L"<body><div class=\"side\"><div class=\"titlebox spacer\">" + temp;
					temp.append(L"</div></div></body>");
					x.sidebar_html = ref new Platform::String(temp.data());
					stuff(x);
					if (x.subredditIndex != -1)
						rules = ApplicationDataHelper::subredditHelpers::trysubredditRulesCache(x.subredditIndex);
					else
						rules = ApplicationDataHelper::subredditHelpers::trysubredditRulesCache(reinterpret_cast<const char16_t*>(x.name->Data()));
					globalvars::reportReasonCache.emplace_back(x.name, rules);

				}
				catch (ApplicationDataHelper::cacheMiss<account::subredditInfo> e)
				{

					e.retrieveTask.then(stuff).then([this]() {
						this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("subInfo"));
						try {
							globalvars::reportReasonCache.emplace_back(_subInfo.name, ApplicationDataHelper::subredditHelpers::trysubredditRulesCache(reinterpret_cast<const char16_t*>(_subInfo.name->Data())));
						}
						catch (ApplicationDataHelper::cacheMiss<std::vector<account::reportReason>> ee)
						{
							Platform::Collections::Vector<account::reportReason>^ rules = ref new Platform::Collections::Vector<account::reportReason>();
							globalvars::reportReasonCache.emplace_back(_subInfo.name, rules);
							ee.retrieveTask.then([rules](std::vector<account::reportReason> v) {
								rules->ReplaceAll(ref new Platform::Array<account::reportReason>(v.data(), v.size()));
							});
						}
					});
				}
				catch (ApplicationDataHelper::cacheMiss<std::vector<account::reportReason>> e)
				{
					Platform::Collections::Vector<account::reportReason>^ rules = ref new Platform::Collections::Vector<account::reportReason>();
					globalvars::reportReasonCache.emplace_back(_subInfo.name, rules);
					e.retrieveTask.then([rules](std::vector<account::reportReason> v) {
						rules->ReplaceAll(ref new Platform::Array<account::reportReason>(v.data(), v.size()));
					});
				}
			}
			else
			{
				_subInfo = _nav->info;
			}
		}
	//pageLoaded = true;
}

void Reddit::SubRedditViewPage::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs ^ e)
{
	if (nav != nullptr)
	{
		nav->sort = _sort;
		nav->rng = _rng;
		nav->info = _subInfo;
	}
}

void Reddit::SubRedditViewPage::updateSidebar()
{
	if (useCss)
	{
		Platform::String^ htmlfinal = L"<head><link href=\"https://www.redditstatic.com/reddit.u3nYlS3TAp8.css\" rel=stylesheet media=all><link href=\"https://www.reddit.com/r/" + _subInfo.name + L"/stylesheet\" rel=stylesheet><style>::after,::before {display: none}.side {float: none !important;background-color: inherit !important;margin: 0px 0px 0 0px !important; padding: 0 !important; max-width: 300px !important;}.side .spacer {margin: 0px 0 0px 0 !important; padding: 0 !important;}.md{max-width:none !important;} body{background-color: transparent !important;}</style></head>" + _subInfo.sidebar_html;
		sidebarWebview->NavigateToString(htmlfinal);
	}
	else
	{
		sidebarWebview->NavigateToString(_subInfo.sidebar_html);
	}
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
	auto clickedPost = static_cast<account::subpostUWP^>(e->ClickedItem);
	if (globalvars::NavState.size() > 1)
	{
		std::vector<navVariant>::const_reverse_iterator rend;
		if (globalvars::NavState.size() > 3)
		{
			rend = globalvars::NavState.crbegin() + 3;
		}
		else
		{
			rend = globalvars::NavState.crend();
		}
		auto res= std::find_if(globalvars::NavState.crbegin(), rend, [&clickedPost](const navVariant& x) {
			try
			{
				if ((x.first == NavTypes::comment) && (x.second.c->pageState != nullptr))
				{
					return x.second.c->postID == clickedPost->helper.id;
				}
				else
				{
					return false;
				}
			}
			catch (Platform::NullReferenceException^)
			{
				__debugbreak();
				return false;
			}
		});
		if (res != rend)
		{
			if (App::CurrentManualFrameContent != nullptr)
			{
				rootWindowGrid::getCurrent()->rootFrame->BackStack->Append(App::CurrentManualFrameContent);
			}
			else
			{
				rootWindowGrid::getCurrent()->rootFrame->BackStack->Append(ref new Windows::UI::Xaml::Navigation::PageStackEntry(rootWindowGrid::getCurrent()->rootFrame->CurrentSourcePageType, static_cast<unsigned char>(static_cast<NavIndexed^>(rootWindowGrid::getCurrent()->rootFrame->Content)->NavigationIndex), nullptr));
			}
			rootWindowGrid::getCurrent()->rootFrame->Content = res->second.c->pageState;
			App::CurrentManualFrameContent = ref new Windows::UI::Xaml::Navigation::PageStackEntry(CommentViewPage::typeid, static_cast<unsigned char>(std::distance(globalvars::NavState.cbegin(), res.base())), nullptr);
			return;
		}
	}
	auto navstuff = new commentNavstate();
	
	navstuff->postID = clickedPost->helper.id;
	navstuff->post = clickedPost;
	((Windows::UI::Xaml::Controls::Frame^)Parent)->Navigate(CommentViewPage::typeid, globalvars::addNav(navstuff));
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
				static Windows::UI::Xaml::DataTemplate^ gridTemp = static_cast<Windows::UI::Xaml::DataTemplate^>(static_cast<App^>(App::Current)->listTemplateRes->Lookup("imageGriddatatemplate"));
				Windows::UI::Xaml::Controls::Primitives::SelectorItem^ newitem;
				newitem = ref new Windows::UI::Xaml::Controls::GridViewItem();
				newitem->ContentTemplate = gridTemp;
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
				static Windows::UI::Xaml::DataTemplate^ gridselfTemp = static_cast<Windows::UI::Xaml::DataTemplate^>(static_cast<App^>(App::Current)->listTemplateRes->Lookup("selfGridTemplate"));
				Windows::UI::Xaml::Controls::Primitives::SelectorItem^ newitem;
				newitem = ref new Windows::UI::Xaml::Controls::GridViewItem();
				newitem->ContentTemplate = gridselfTemp;

				newitem->Tag = (unsigned char)displayType::self;
				args->ItemContainer = newitem;
			}
		}
	}
	args->IsContainerPrepared = true;
}


void Reddit::SubRedditViewPage::AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_subInfo.key_color = Windows::UI::Colors::Red;
	Bindings->Update();
}
