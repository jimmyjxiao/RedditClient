//
// rootWindowGrid.xaml.cpp
// Implementation of the rootWindowGrid class
//

#include "pch.h"
#include "rootWindowGrid.xaml.h"
#include "SubRedditViewPage.xaml.h"
#include "ApplicationDataHelper.h"
#include "UserPage.xaml.h"
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
const account::AccountInfo lastItem =
{
	nullptr,
	INT_MIN,
	INT_MIN
};
bool rootWindowGrid::Has_mail::get()
{
	//return globalvars::currentacc->me.has_mail;
	return true;
}
rootWindowGrid::rootWindowGrid()
{
	singleT = this;
	InitializeComponent();
	auto coretitle = Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->TitleBar;
	Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->TitleBar->ButtonBackgroundColor = Windows::UI::Colors::Transparent;
	searchIcon = ref new Windows::UI::Xaml::Controls::SymbolIcon(Windows::UI::Xaml::Controls::Symbol::Find);
	goIcon = ref new Windows::UI::Xaml::Controls::SymbolIcon(Windows::UI::Xaml::Controls::Symbol::Forward);
	coretitle->ExtendViewIntoTitleBar = true;
	coretitle->IsVisibleChanged += ref new Windows::Foundation::TypedEventHandler<Windows::ApplicationModel::Core::CoreApplicationViewTitleBar ^, Platform::Object^>(this, &rootWindowGrid::CoreTitleBar_IsVisibleChanged);
	coretitle->LayoutMetricsChanged += ref new Windows::Foundation::TypedEventHandler<Windows::ApplicationModel::Core::CoreApplicationViewTitleBar ^, Platform::Object^>(this, &rootWindowGrid::CoreTitleBar_LayoutMetricsChanged);
	auto credvault = ref new Windows::Security::Credentials::PasswordVault();
	try
	{
		auto acccreds = credvault->FindAllByResource("reddit_refresh_token");
		for (auto x : acccreds)
		{
			x->RetrievePassword();
			account::AccountInterface* accclass;
			try
			{
				accclass = new account::AccountInterface(x->Password, ApplicationDataHelper::userHelpers::MyUserInfoCache(x->UserName));
			}
			catch (std::exception)
			{
				accclass = new account::AccountInterface(x->Password);
			}
			globalvars::accounts.insert(std::make_pair(x->UserName->Data(), accclass));
			globalvars::currentacc = accclass;
		}
	}
	catch (...)
	{

		OutputDebugString(L"No users logged in");
		account::AccountInterface::LoginNewAcc().then([this](account::AccountInterface*a) {
			globalvars::currentacc = a;
			globalvars::accounts.emplace(a->me.username->Data(), a);
			Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				ref new Windows::UI::Core::DispatchedHandler([this]()
			{
				rootFrame->Navigate(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate(nullptr)));
			})); 
		});
	}
	globalvars::UpdateAccountsTask().then([this]() {
		this->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
			this->updateAccountsToComboBox();
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Has_mail"));
		}));
	});
	if (globalvars::currentacc != nullptr)
	{
		rootFrame->Navigate(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate(nullptr)));
	}
}
rootWindowGrid^ Reddit::rootWindowGrid::singleT;
rootWindowGrid ^ Reddit::rootWindowGrid::getCurrent()
{
	return singleT;
}

void Reddit::rootWindowGrid::updateAccountsToComboBox()
{
	Platform::Collections::Vector<account::AccountInfo> ^accounts = ref new Platform::Collections::Vector<account::AccountInfo>(globalvars::accounts.size() );
	std::transform(globalvars::accounts.begin(), globalvars::accounts.end(), Windows::Foundation::Collections::begin(accounts), [](std::pair<std::wstring, account::AccountInterface*> pair) {
		return pair.second->me;
	});
	accounts->Append(lastItem);
	accountBox->ItemsSource = accounts;
	accountBox->SelectedItem = globalvars::currentacc->me;
}

void Reddit::rootWindowGrid::NavigateToNewPage(const Windows::UI::Xaml::Interop::TypeName&const type, Platform::Object ^ args)
{
	rootFrame->Navigate(type, args);
	if (App::CurrentManualFrameContent != nullptr)
	{
		rootFrame->BackStack->Append(App::CurrentManualFrameContent);
		App::CurrentManualFrameContent = nullptr;
	}
}

bool Reddit::rootWindowGrid::wstrValidGo(const wchar_t * str, size_t size)
{
	if (size == 0)
		return false;
	for (size_t s = 0; s < size; ++s)
	{
		if (!iswalnum(str[s]) && (str[s] != L'-') && (str[s] != L'_'))
			return false;
	}
	return true;
}

void Reddit::rootWindowGrid::CoreTitleBar_IsVisibleChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar ^ sender, Platform::Object ^ args)
{
	//__debugbreak();
}

void Reddit::rootWindowGrid::CoreTitleBar_LayoutMetricsChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar ^ sender, Platform::Object ^ args)
{
	titleRowDef->Height = sender->Height;
	Window::Current->SetTitleBar(titleRec);
	searchBox->Margin = Windows::UI::Xaml::ThicknessHelper::FromLengths(0, 0, sender->SystemOverlayRightInset, 0);
	//titleGrid->Padding = Windows::UI::Xaml::ThicknessHelper::FromLengths(sender->SystemOverlayLeftInset, 0, sender->SystemOverlayRightInset, 0);
}


void Reddit::rootWindowGrid::AutoSuggestBox_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	if (args->Reason == Windows::UI::Xaml::Controls::AutoSuggestionBoxTextChangeReason::UserInput)
	{
		auto wstr = sender->Text->Data();
		if (wstr[0] == L'/')
		{
			if ((wstr[1] == L'u' || wstr[1] == L'r') && (wstr[2] == L'/'))
			{
				_isSearch = !wstrValidGo(wstr + 3, sender->Text->Length() - 3);
				if (wstr[1] == L'r' && !_isSearch)
				{
					sender->ItemsSource = ApplicationDataHelper::subredditHelpers::SearchSubNames(reinterpret_cast<const char16_t*>(wstr + 3), false);
				}
				else
				{
					sender->ItemsSource = nullptr;
				}
			}
			else
			{
				_isSearch = true;
				sender->ItemsSource = nullptr;
			}
		}
		else if (wstr[0] == L'r' || wstr[0] == L'u')
		{
			if (wstr[1] == L'/')
			{
				_isSearch = !wstrValidGo(wstr + 2, sender->Text->Length() - 2);
				if (wstr[0] == L'r' && !_isSearch)
				{
					sender->ItemsSource = ApplicationDataHelper::subredditHelpers::SearchSubNames(reinterpret_cast<const char16_t*>(wstr + 2), false);
				}
				else
				{
					sender->ItemsSource = nullptr;
				}
			}
			else
			{
				_isSearch = true;
				sender->ItemsSource = ApplicationDataHelper::subredditHelpers::SearchSubNames(reinterpret_cast<const char16_t*>(wstr), true);
			}
		}
		else
		{
			_isSearch = true;
			sender->ItemsSource = ApplicationDataHelper::subredditHelpers::SearchSubNames(reinterpret_cast<const char16_t*>(wstr), true);
		}
		if (_isSearch)
		{
			sender->QueryIcon = searchIcon;
		}
		else
		{
			sender->QueryIcon = goIcon;
		}
	}

	
	
}


void Reddit::rootWindowGrid::AutoSuggestBox_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args)
{
	if (args->ChosenSuggestion == nullptr)
	{
		auto wstr = sender->Text->Data();
		if (wstr[0] == L'/')
		{
			if (wstr[2] == L'/')
			{
				if (wstr[1] == L'u')
				{
					if (wstrValidGo(wstr + 3, sender->Text->Length() - 3))
					{

					}
				}
				else if (wstr[1] == L'r')
				{
					if (wstrValidGo(wstr + 3, sender->Text->Length() - 3))
					{
						rootFrame->Navigate(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate{ ref new Platform::String(wstr + 3, sender->Text->Length() - 3) }));
					}
				}
			}

		}
		else if (wstr[0] == L'r' && wstr[1] == L'/' && (wstrValidGo(wstr + 2, sender->Text->Length() - 2)))
		{
			rootFrame->Navigate(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate(ref new Platform::String(wstr + 2, sender->Text->Length() - 2) )));
		}
	}
	else
	{
		rootFrame->Navigate(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate{static_cast<account::subredditInfo>(args->ChosenSuggestion)}));
	}
}


void Reddit::rootWindowGrid::rootFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	if (rootFrame->CanGoBack)
	{
		Windows::UI::Core::SystemNavigationManager::GetForCurrentView()->AppViewBackButtonVisibility =
			Windows::UI::Core::AppViewBackButtonVisibility::Visible;
	}
	else
	{
		Windows::UI::Core::SystemNavigationManager::GetForCurrentView()->AppViewBackButtonVisibility =
			Windows::UI::Core::AppViewBackButtonVisibility::Collapsed;
	}
}


void Reddit::rootWindowGrid::Grid_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	updateAccountsToComboBox();
}

Windows::UI::Xaml::DataTemplate ^Reddit::comboboxTemplateSelector::SelectTemplateCore(Platform::Object ^ item, Windows::UI::Xaml::DependencyObject ^ container)
{
	if (dynamic_cast<Windows::UI::Xaml::Controls::ContentPresenter^>(container) != nullptr)
	{
		return closedTemplate;
	}
	else if (dynamic_cast<Windows::UI::Xaml::Controls::ComboBoxItem^>(container) != nullptr)
	{
		const auto & info = static_cast<account::AccountInfo>(item);
		if ((info.comment_karma == lastItem.comment_karma) && (info.link_karma == lastItem.link_karma) && (info.username == lastItem.username))
		{
			return nullTemplate;
		}
		else
			return listTemplate;
	}
}


void Reddit::rootWindowGrid::ComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	static bool ignore = false;
	if (!ignore)
	{
		if (e->AddedItems->Size > 0)
		{
			const auto & info = static_cast<account::AccountInfo>(e->AddedItems->GetAt(0));
			if ((info.comment_karma == lastItem.comment_karma) && (info.link_karma == lastItem.link_karma) && (info.username == lastItem.username))
			{


				account::AccountInterface::LoginNewAcc().then([sender](concurrency::task<account::AccountInterface*> n) {
					try
					{
						const auto acc = n.get();
						const auto & s = static_cast<Platform::Collections::Vector<account::AccountInfo>^>(static_cast<Windows::UI::Xaml::Controls::ComboBox^>(sender)->ItemsSource);
						s->InsertAt(s->Size - 1, acc->me);
						globalvars::accounts.emplace(acc->me.username->Data(), std::move(acc));
					}
					catch (account::NewAccountLoginFailure &e)
					{
						switch (e.Reason)
						{
						case account::NewAccountLoginFailure::reason::UserCanceled:break;
						case account::NewAccountLoginFailure::reason::networkerror:
						{
							(ref new Windows::UI::Popups::MessageDialog(ref new Platform::String(e.message.data()), "Network Error"))->ShowAsync();
						}
						}
					}
				});
				ignore = true;
				try
				{
					static_cast<Windows::UI::Xaml::Controls::ComboBox^>(sender)->SelectedItem = e->RemovedItems->GetAt(0);
				}
				catch (Platform::Exception^ e)
				{
					__debugbreak();
					static_cast<Windows::UI::Xaml::Controls::ComboBox^>(sender)->SelectedItem = globalvars::currentacc->me;
				}
			}
			else
			{
				auto it = globalvars::accounts.find(info.username->Data());
				if (it != globalvars::accounts.end())
				{
					globalvars::currentacc = it->second;
				}
				else
				{
					__debugbreak();
				}
			}
		}
	}
	else
	{
		ignore = false;
	}
}



void Reddit::rootWindowGrid::Flyout_Opening(Platform::Object^ sender, Platform::Object^ e)
{
	/*const auto child = static_cast<Reddit::MixedRedditTypeControl^>(static_cast<Windows::UI::Xaml::Controls::Flyout^>(sender)->Content);
	if (child->redditContents == nullptr)
	{
		globalvars::currentacc->getMessages().then([child](std::pair<Platform::Collections::Vector<account::IRedditTypeIdentifier^>^, const Platform::String^> r) {
			child->redditContents = r.first;
		});
	}*/
}


void Reddit::rootWindowGrid::Flyout_Open(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Windows::UI::Xaml::Controls::Primitives::FlyoutBase::ShowAttachedFlyout(static_cast<NoToggleBehaviorButton^>(sender));
}

void Reddit::rootWindowGrid::Hyperlink_Click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args)
{
	auto nav = new UserNavState();
	nav->account = static_cast<account::AccountInfo>(static_cast<Windows::UI::Xaml::Controls::TextBlock^>(VisualTreeHelper::GetParent(sender))->DataContext);
	if (!TryNavigateToCachedPage([&nav](const navVariant& x) {
		return (x.first.Name == ((Windows::UI::Xaml::Interop::TypeName)(UserPage::typeid)).Name && nav->account.username == static_cast<UserNavState*>(x.second)->account.username);
	}))
		NavigateToNewPage(UserPage::typeid, globalvars::addNav(UserPage::typeid, std::move(nav)));
}
