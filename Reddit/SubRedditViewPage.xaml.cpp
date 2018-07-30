//
// SubRedditViewPage.xaml.cpp
// Implementation of the SubRedditViewPage class
//

#include "pch.h"
#include "SubRedditViewPage.xaml.h"
#include "CommentViewPage.xaml.h"
#include "ApplicationDataHelper.h"

#include "MyResources.xaml.h"
#define _NAV static_cast<subredditNavstate*>(baseRef)
#define _sort _NAV->sort
#define _subInfo _NAV->info
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

std::vector<Windows::UI::Xaml::Controls::Primitives::SelectorItem^> Reddit::SubRedditViewPage::ContentGridItemRecycle;
std::vector<Windows::UI::Xaml::Controls::Primitives::SelectorItem^> Reddit::SubRedditViewPage::SelfGridItemRecycle;
std::vector<Windows::UI::Xaml::Controls::Primitives::SelectorItem^> Reddit::SubRedditViewPage::AdGridItemRecycle;
std::vector<Windows::UI::Xaml::Controls::ListViewItem^> Reddit::SubRedditViewPage::ListPostItemRecycle;
std::vector<Windows::UI::Xaml::Controls::ListViewItem^> Reddit::SubRedditViewPage::ListAdItemRecycle;
void Reddit::SubRedditViewPage::NavigateToSubreddit(Platform::String ^ sub, account::postSort sort, account::timerange range)
{
	account::subredditInfo* premadeInfo = nullptr;
	if (sub != nullptr && sort == account::postSort::Defaultsort)
	{
		sort = account::postSort::hot;
	}
	if (!rootWindowGrid::TryNavigateToCachedPage([&sub, &sort, &range, &premadeInfo](const navVariant & a) {
		if (a.first.Name == ((Windows::UI::Xaml::Interop::TypeName)(SubRedditViewPage::typeid)).Name)
		{
			auto x = static_cast<subredditNavstate*>(a.second);
			if (x->info.name == sub)
			{
				premadeInfo = &x->info;
				if ((x->rng == range) && (x->sort == sort))
					return true;
				else
					return false;
			}
		}
		else
		{
			return false;
		}
	}))
	{
		if (premadeInfo != nullptr)
		{
			rootWindowGrid::getCurrent()->NavigateToNewPage(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate(*premadeInfo, std::move(sort), std::move(range))));
		}
		else
		{
			rootWindowGrid::getCurrent()->NavigateToNewPage(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate(std::move(sub), std::move(sort), std::move(range))));
		}
	}
	
}
void Reddit::SubRedditViewPage::NavigateToSubreddit(account::subredditInfo sub, account::postSort sort, account::timerange range)
{
	if (sub.name != nullptr && sort == account::postSort::Defaultsort)
	{
		sort = account::postSort::hot;
	}
	if (!rootWindowGrid::TryNavigateToCachedPage([i = sub.subredditIndex, &sort, &range](const navVariant & a) {
		if (a.first.Name == ((Windows::UI::Xaml::Interop::TypeName)(SubRedditViewPage::typeid)).Name)
		{
			auto x = static_cast<subredditNavstate*>(a.second);
			return ((x->info.subredditIndex == i) && (x->rng == range) && (x->sort == sort));
		}
		else
		{
			return false;
		}
	})) {
		rootWindowGrid::getCurrent()->NavigateToNewPage(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate(std::move(sub), sort, range)));
	}
}
Reddit::SubRedditViewPage::~SubRedditViewPage()
{
	//__debugbreak();
}
Windows::Foundation::Collections::IVector<account::subpostUWP^>^ SubRedditViewPage::posts::get()
{
	return lPtr->listing;
}
SubRedditViewPage::SubRedditViewPage()
{
	Resources->Insert(ref new Platform::String(L"reportReasonsResource"),ref new Platform::String(L"dheh"));
	InitializeComponent();
}
account::timerange SubRedditViewPage::Range::get()
{
	return _NAV->rng;
}
bool SubRedditViewPage::SidebarUseCSS::get()
{
	return useCss;
}
Platform::String^ Reddit::SubRedditViewPage::Subreddit::get()
{
	return _NAV->info.name;
}
account::subredditInfo Reddit::SubRedditViewPage::subInfo::get()
{
	return _subInfo;
}
void SubRedditViewPage::SidebarUseCSS::set(bool a)
{
	if (a != useCss)
	{
		useCss = a;
		updateSidebar();
	}
}
bool Reddit::SubRedditViewPage::Subscribed::get()
{
	return (_subInfo.subscribed & globalvars::currentacc->me.bitflag);
}
void Reddit::SubRedditViewPage::Subscribed::set(bool a)
{
	if (a != Subscribed)
	{
		__debugbreak();
	}
}
void Reddit::SubRedditViewPage::Range::set(account::timerange newrange)
{
	if (newrange != _NAV->rng && pageLoaded)
	{

		lPtr = globalvars::currentacc->getsubredditAsyncVec(_subreddit, _sort, newrange, RefreshSourceAndGetNewToken());
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("posts"));
		_NAV->rng = newrange;
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Range"));
	}
}
account::postSort Reddit::SubRedditViewPage::Sort::get()
{
	return _sort;
}
void Reddit::SubRedditViewPage::Sort::set(account::postSort newsort)
{
	if (newsort != _sort && pageLoaded)
	{
		//timeSelector->SelectedIndex = 5;
		if (newsort == account::postSort::controversial || newsort == account::postSort::top)
		{
			_NAV->rng = account::timerange::all;
			lPtr = globalvars::currentacc->getsubredditAsyncVec(_subreddit, newsort, _NAV->rng, RefreshSourceAndGetNewToken());
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("posts"));
		}
		else
		{
			lPtr = globalvars::currentacc->getsubredditAsyncVec(_subreddit, newsort, account::timerange::Default, RefreshSourceAndGetNewToken());
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("posts"));
		}
		_sort = newsort;
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Sort"));
	}
}


void Reddit::SubRedditViewPage::OnNavigatedToPageCode()
{
	_subreddit = _NAV->info.name;
	if (_subreddit == nullptr)
	{
		_subInfo.pname = L"Frontpage";
		
		auto bestoption = ref new TextBlock();
		bestoption->Text = "Best";
		bestoption->Tag = account::postSort::Defaultsort;
		sortSelector->Items->InsertAt(0, std::move(bestoption));
		_sort = account::postSort::Defaultsort;
		sortSelector->SelectedIndex = 0;
		//mystate->IsActive = false;
		goto notrealsubs;
	}
	else if (_subreddit == L"all" || _subreddit == L"popular")
	{
		_subInfo.pname = L"/r/" + _subreddit;
	notrealsubs:
		sideBarButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		_subInfo.key_color = Windows::UI::Colors::Black;
		commandBar->Background = static_cast<Windows::UI::Xaml::Media::SolidColorBrush^>(Application::Current->Resources->Lookup("SystemControlBackgroundChromeMediumBrush"));
		subInfopanel->Children->RemoveAtEnd();
		subInfopanel->Children->RemoveAtEnd();
		pageLoaded = true;
		lPtr = globalvars::currentacc->getsubredditAsyncVec(_subreddit, _sort, _NAV->rng, refreshCancelationSource.get_token());

	}
	else
	{
		lPtr = globalvars::currentacc->getsubredditAsyncVec(_subreddit, _sort, _NAV->rng, refreshCancelationSource.get_token());
		int rngIndex;
		if (((int)_NAV->rng) == 6)
			rngIndex = 3;
		else
			rngIndex = (int)_NAV->rng;
		timeSelector->SelectedIndex = rngIndex;
		sortSelector->SelectedIndex = (int)_sort;
		pageLoaded = true;
		if (_NAV->info.subredditIndex == INT_MAX)
		{
			try
			{
				_subInfo = ApplicationDataHelper::subredditHelpers::trysubredditInfoCache((const char16_t*)_subreddit->Data(), TaskCancellationSource.get_token());
				Platform::Collections::Vector<account::reportReason>^ rules;
				std::wstring temp = _subInfo.sidebar_html->Data();
				temp.insert(27, L"usertext usertext-body md-container may-blank-within ");
				temp = L"<body><div class=\"side\"><div class=\"titlebox spacer\">" + temp;
				temp.append(L"</div></div></body>");
				_subInfo.sidebar_html = ref new Platform::String(temp.data());
				this->updateSidebar();
				if (_subInfo.submissions == account::subType::self)
				{
					listingType = ViewMode::list;
				}
				else
				{
					listingType = ViewMode::grid;
				}
				if (_subInfo.subredditIndex != INT_MIN)
					rules = ApplicationDataHelper::subredditHelpers::trysubredditRulesCache(_subInfo.subredditIndex, ref new Platform::Collections::Vector<account::reportReason>(), TaskCancellationSource.get_token());
				else
					rules = ApplicationDataHelper::subredditHelpers::trysubredditRulesCache(reinterpret_cast<const char16_t*>(_subInfo.name->Data()), ref new Platform::Collections::Vector<account::reportReason>(), TaskCancellationSource.get_token());
				globalvars::reportReasonCache.emplace_back(_subInfo.name, rules);

			}
			catch (ApplicationDataHelper::cacheMiss<account::subredditInfo> e)
			{

				e.retrieveTask.then([this](account::subredditInfo z) {
					_subInfo = std::move(z);
					this->updateSidebar();
					if (_subInfo.submissions == account::subType::self)
					{
						listingType = ViewMode::list;
					}
					else
					{
						listingType = ViewMode::grid;
					}
					this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("subInfo"));
					this->PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Subscribed"));
					try {
						globalvars::reportReasonCache.emplace_back(_subInfo.name, ApplicationDataHelper::subredditHelpers::trysubredditRulesCache(reinterpret_cast<const char16_t*>(_subInfo.name->Data()), ref new Platform::Collections::Vector<account::reportReason>(), TaskCancellationSource.get_token()));
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
	if (!rootWindowGrid::getCurrent()->TryNavigateToCachedPage([&clickedPost](const navVariant& x) {
		try
		{
			if ((x.first.Name == ((Windows::UI::Xaml::Interop::TypeName)(SubRedditViewPage::typeid)).Name) && (x.second->pageState != nullptr))
			{
				return static_cast<commentNavstate*>(x.second)->postID == clickedPost->helper.getId();
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
	}))
	{
		auto navstuff = new commentNavstate();
		navstuff->postID = clickedPost->helper.getId();
		navstuff->post = clickedPost;
		rootWindowGrid::getCurrent()->NavigateToNewPage(CommentViewPage::typeid, globalvars::addNav(CommentViewPage::typeid, navstuff));
	}
	
}


void Reddit::SubRedditViewPage::listGrid_ChoosingItemContainer(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ChoosingItemContainerEventArgs^ args)
{
	const auto &item = static_cast<account::subpostUWP^>(args->Item);
	static Windows::UI::Xaml::DataTemplate^ adTemplate = static_cast<Windows::UI::Xaml::DataTemplate^>(static_cast<App^>(App::Current)->listTemplateRes->Lookup("GridBannerAd"));
	if (item == nullptr)
	{
		args->ItemContainer = ref new Windows::UI::Xaml::Controls::GridViewItem();
		args->ItemContainer->Tag = (unsigned char)displayType::ad;
		args->ItemContainer->ContentTemplate = adTemplate;
	}
	else if(!item->self)
	{
		if (args->ItemContainer != nullptr)
		{
			auto displaytype = static_cast<displayType>(static_cast<unsigned char>(args->ItemContainer->Tag));
			if (displaytype != displayType::content)
			{
				if (displaytype == displayType::self)
					SelfGridItemRecycle.push_back(args->ItemContainer);
				else if (displaytype == displayType::ad)
					AdGridItemRecycle.push_back(args->ItemContainer);
				goto retrievecontentTem;
			}
			else
			{
				auto it = std::find(ContentGridItemRecycle.begin(), ContentGridItemRecycle.end(), args->ItemContainer);
				if (it != ContentGridItemRecycle.end())
				{
					std::swap(*it, ContentGridItemRecycle.back());
					ContentGridItemRecycle.pop_back();
				}
			}
		}
		else
		{
		retrievecontentTem:
			if (ContentGridItemRecycle.size() > 0)
			{
				args->ItemContainer = ContentGridItemRecycle.back();
				ContentGridItemRecycle.pop_back();
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
					ContentGridItemRecycle.push_back(args->ItemContainer);
				else if (displaytype == displayType::ad)
					AdGridItemRecycle.push_back(args->ItemContainer);
				goto retrieveselfTem;
			}
			else
			{
				auto it = std::find(SelfGridItemRecycle.begin(), SelfGridItemRecycle.end(), args->ItemContainer);
				if (it != SelfGridItemRecycle.end())
				{
					std::swap(*it, SelfGridItemRecycle.back());
					SelfGridItemRecycle.pop_back();
				}
			}
		}
		else
		{
		retrieveselfTem:
			if (SelfGridItemRecycle.size() > 0)
			{
				args->ItemContainer = SelfGridItemRecycle.back();
				SelfGridItemRecycle.pop_back();
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


void Reddit::SubRedditViewPage::listView_ChoosingItemContainer(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ChoosingItemContainerEventArgs^ args)
{
	static Windows::UI::Xaml::DataTemplate^ postTemplate = static_cast<Windows::UI::Xaml::DataTemplate^>(static_cast<App^>(App::Current)->listTemplateRes->Lookup("listSelfPost"));
	static Windows::UI::Xaml::DataTemplate^ adTemplate = static_cast<Windows::UI::Xaml::DataTemplate^>(static_cast<App^>(App::Current)->listTemplateRes->Lookup("inlineNativeAd"));
	if (args->Item != nullptr)
	{
		if (args->ItemContainer == nullptr)
		{
			goto newpostContainer;
		}
		if (static_cast<bool>(args->ItemContainer->Tag) == true)
		{
			auto it = std::find(ListPostItemRecycle.begin(), ListPostItemRecycle.end(), args->ItemContainer);
			if (it != ListPostItemRecycle.end())
			{
				std::iter_swap(it, ListPostItemRecycle.end() - 1);
				ListPostItemRecycle.pop_back();
			}
		}
		else
		{
			if (ListPostItemRecycle.size() > 0)
			{
				args->ItemContainer = ListPostItemRecycle.back();
				ListPostItemRecycle.pop_back();
			}
			else
			{
				newpostContainer:
				args->ItemContainer = ref new Windows::UI::Xaml::Controls::ListViewItem();
				args->ItemContainer->Tag = true;
				args->ItemContainer->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
				args->ItemContainer->HorizontalContentAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
				args->ItemContainer->ContentTemplate = postTemplate;
			}
		}
	}
	else
	{
		if (args->ItemContainer == nullptr)
		{
			goto newadcontainer;
		}
		if (static_cast<bool>(args->ItemContainer->Tag) == false)
		{
			auto it = std::find(ListAdItemRecycle.begin(), ListAdItemRecycle.end(), args->ItemContainer);
			if (it != ListAdItemRecycle.end())
			{
				std::iter_swap(it, ListAdItemRecycle.end() - 1);
				ListAdItemRecycle.pop_back();
			}
		}
		else
		{
			if (ListAdItemRecycle.size() > 0)
			{
				args->ItemContainer = ListAdItemRecycle.back();
				ListAdItemRecycle.pop_back();
			}
			else
			{
				newadcontainer:
				args->ItemContainer = ref new Windows::UI::Xaml::Controls::ListViewItem();
				args->ItemContainer->Tag = false;
				args->ItemContainer->ContentTemplate = adTemplate;
			}
		}
	}
	args->IsContainerPrepared = true;
}


void Reddit::SubRedditViewPage::postButton_click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (SubmitPopup == nullptr)
		SubmitPopup = ref new NewPostPopup();
	SubmitPopup->Subreddit = _subreddit;
	SubmitPopup->ShowAsync();
}

Platform::Object ^ Reddit::SubscribersConverter::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
{
	wchar_t buffer[26];
	wchar_t numbuf[9];
	_itow_s(static_cast<unsigned int>(value), numbuf, 10);
	static NUMBERFMTW numbfmt =
	{
		0,
		0,
		3,
		L"",
		L",",
		0
	};
	GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, numbuf, &numbfmt, buffer, 19);
	auto err = wcscat_s(buffer, L" Subscribers");
	return ref new Platform::String(buffer);
}
